#include "config.h"

#include "conn.h"

// TODO: make pull to json-parser
#include "json.h"

#include <dbus/dbus-glib.h>
#include <rest/rest-proxy.h>
#include <string.h>
// TODO: drop libsoup. http://vk.com/support?act=show&id=7863767
#include <libsoup/soup.h>

#ifdef ENABLE_DEBUG
#include <locale.h>
#endif

#include <telepathy-glib/telepathy-glib.h>

#include "contact-list.h"
#include "protocol.h"

static void
init_aliasing (gpointer,
               gpointer);

G_DEFINE_TYPE_WITH_CODE (DurkaConnection,
    durka_connection,
    TP_TYPE_BASE_CONNECTION,
    G_IMPLEMENT_INTERFACE (TP_TYPE_SVC_CONNECTION_INTERFACE_ALIASING,
      init_aliasing);
    G_IMPLEMENT_INTERFACE (TP_TYPE_SVC_CONNECTION_INTERFACE_CONTACTS,
      tp_contacts_mixin_iface_init);
    G_IMPLEMENT_INTERFACE (TP_TYPE_SVC_CONNECTION_INTERFACE_CONTACT_LIST,
      tp_base_contact_list_mixin_list_iface_init);
    G_IMPLEMENT_INTERFACE (TP_TYPE_SVC_CONNECTION_INTERFACE_CONTACT_GROUPS,
      tp_base_contact_list_mixin_groups_iface_init);
    G_IMPLEMENT_INTERFACE (TP_TYPE_SVC_CONNECTION_INTERFACE_CONTACT_BLOCKING,
      tp_base_contact_list_mixin_blocking_iface_init);
    G_IMPLEMENT_INTERFACE (TP_TYPE_SVC_CONNECTION_INTERFACE_PRESENCE,
      tp_presence_mixin_iface_init);
    G_IMPLEMENT_INTERFACE (TP_TYPE_SVC_CONNECTION_INTERFACE_SIMPLE_PRESENCE,
      tp_presence_mixin_simple_presence_iface_init))

enum {
  PROP_ACCOUNT = 1,
  PROP_PASSWORD,
  PROP_TOKEN,
  N_PROPS
};

typedef struct {
  RestProxy *rest;
  TpBaseConnection *conn;
  gchar *token;
  gchar *server;
  gchar *key;
  gchar *ts;
  GTimer *timer;
  gboolean exit;
  SoupSession *session;
  SoupMessage *msg;
} PollData;

struct _DurkaConnectionPrivate {
  gchar *account;
  gchar *password;
  gchar *token;
  RestProxy *rest;
  GThread *thread;
  DurkaContactList *contact_list;
  gboolean away;
  gboolean exit;
  gpointer data;
};

static void
durka_connection_init (DurkaConnection *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, DURKA_TYPE_CONNECTION, DurkaConnectionPrivate);
}

static void
get_property (GObject *object,
              guint property_id,
              GValue *value,
              GParamSpec *spec)
{
  DurkaConnection *self = DURKA_CONNECTION (object);

  switch (property_id) {
    case PROP_ACCOUNT:
      g_value_set_string (value, self->priv->account);
      break;

    case PROP_PASSWORD:
      g_value_set_string (value, self->priv->password);
      break;

    case PROP_TOKEN:
      g_value_set_string (value, self->priv->token);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, spec);
  }
}

static void
set_property (GObject *object,
              guint property_id,
              const GValue *value,
              GParamSpec *spec)
{
  DurkaConnection *self = DURKA_CONNECTION (object);

  switch (property_id) {
    case PROP_ACCOUNT:
      g_free (self->priv->account);
      self->priv->account = g_value_dup_string (value);
      break;

    case PROP_PASSWORD:
      g_free (self->priv->password);
      self->priv->password = g_value_dup_string (value);
      break;

    case PROP_TOKEN:
      g_free (self->priv->token);
      self->priv->token = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, spec);
  }
}

static void
finalize (GObject *object)
{
  DurkaConnection *self = DURKA_CONNECTION (object);

  tp_contacts_mixin_finalize (object);
  g_free (self->priv->account);
  g_free (self->priv->password);
  g_free (self->priv->token);

  G_OBJECT_CLASS (durka_connection_parent_class)->finalize (object);
}

static gchar *
get_unique_connection_name (TpBaseConnection *conn)
{
  DurkaConnection *self = DURKA_CONNECTION (conn);

  return g_strdup_printf ("%s@%p", self->priv->account, self);
}

gchar *
durka_contact_list_normalize_contact (TpHandleRepoIface *repo,
                                      const gchar *id,
                                      gpointer context,
                                      GError **error)
{
  gchar *normal = NULL;

  if (durka_protocol_check_contact_id (id, &normal, error))
    return normal;
  else
    return NULL;
}

static void
create_handle_repos (TpBaseConnection *conn,
                     TpHandleRepoIface *repos[TP_NUM_HANDLE_TYPES])
{
  repos[TP_HANDLE_TYPE_CONTACT] = tp_dynamic_handle_repo_new (TP_HANDLE_TYPE_CONTACT,
                                                              durka_contact_list_normalize_contact,
                                                              NULL);
}

static void
alias_updated_cb (DurkaContactList *contact_list,
                  TpHandle contact,
                  DurkaConnection *self)
{
  GPtrArray *aliases;
  GValueArray *pair;

  pair = tp_value_array_build (2, G_TYPE_UINT, contact, G_TYPE_STRING,
                               durka_contact_list_get_alias (contact_list, contact),
                               G_TYPE_INVALID);

  aliases = g_ptr_array_sized_new (1);
  g_ptr_array_add (aliases, pair);

  tp_svc_connection_interface_aliasing_emit_aliases_changed (self, aliases);

  g_ptr_array_unref (aliases);
  g_value_array_free (pair);
}

static void
presence_updated_cb (DurkaContactList *contact_list,
                     TpHandle contact,
                     DurkaConnection *self)
{
  TpBaseConnection *base = (TpBaseConnection *) self;
  TpPresenceStatus *status;

  /* we ignore the presence indicated by the contact list for our own handle */
  if (contact == tp_base_connection_get_self_handle (base))
    return;

  status = tp_presence_status_new (durka_contact_list_get_presence (contact_list, contact), NULL);
  tp_presence_mixin_emit_one_presence_update ((GObject *) self, contact, status);
  tp_presence_status_free (status);
}

static GPtrArray *
create_channel_managers (TpBaseConnection *conn)
{
  DurkaConnection *self = DURKA_CONNECTION (conn);
  GPtrArray *ret = g_ptr_array_sized_new (1);

  self->priv->contact_list = DURKA_CONTACT_LIST (
      g_object_new ( DURKA_TYPE_CONTACT_LIST, "connection", conn, "simulation-delay", 100, NULL));

  g_signal_connect(self->priv->contact_list, "alias-updated", G_CALLBACK (alias_updated_cb), self);
  g_signal_connect(self->priv->contact_list, "presence-updated", G_CALLBACK (presence_updated_cb), self);

  g_ptr_array_add (ret, self->priv->contact_list);

  return ret;
}

gint
invoke_vk_api (DurkaConnection *self,
               const gchar *method,
               json_value **response,
               GError **error,
               ...)
{
  va_list params;
  const gchar *key = NULL;
  const gchar *value = NULL;

  RestProxyCall *call = rest_proxy_new_call (self->priv->rest);
  rest_proxy_call_set_function (call, method);
  rest_proxy_call_set_method (call, "POST");
  rest_proxy_call_add_params (call, "access_token", self->priv->token, "format", "json", "v", "5.2", NULL);

  va_start (params, error);
  while ((key = va_arg (params, const gchar *)) != NULL) {
    value = va_arg (params, const gchar *);
    rest_proxy_call_add_param (call, key, value);
  }
  va_end (params);

  if (!rest_proxy_call_sync (call, error)) {
    g_error ("Cannot make call: %s", (*error)->message);
    tp_base_connection_change_status (TP_BASE_CONNECTION (self), TP_CONNECTION_STATUS_DISCONNECTED,
                                      TP_CONNECTION_STATUS_REASON_NETWORK_ERROR);
    g_object_unref (call);
    return -1;
  }

  gchar *ret = NULL;
  ret = (gchar *) rest_proxy_call_get_payload (call);

  g_assert (g_utf8_validate (ret, -1, NULL));

#ifdef ENABLE_DEBUG
  g_print ("From %s: %s\n", method, ret);
#endif

  *response = json_parse (ret, strlen (ret));
  g_object_unref (call);

  g_assert ((*response)->type == json_object);

  if (*error)
    g_error_free (*error);

  if (json_value_find (*response, "response"))
    return 0;

  json_value *l_error = json_value_find (*response, "error");
  *error = g_error_new (1, json_value_find (l_error, "error_code")->u.integer, "%s",
                        json_value_find (l_error, "error_msg")->u.string.ptr);
  return (*error)->code;
}

static gboolean
polling (const gpointer data)
{
  PollData *poll = data;
  json_value *parsed;
  json_value *response;
  json_value *updates;
  json_value *temp;
  GError *resterror = NULL;
  gboolean failed = TRUE;
  guint event_code = 0;
  gint user_id = NULL;
  gboolean first_time = TRUE;

  g_timer_start (poll->timer);

  while (!poll->exit) {
    if (failed) {
      /* Getting data required for connection to a Long Poll server
       * https://vk.com/dev/messages.getLongPollServer */
      if (invoke_vk_api (DURKA_CONNECTION (poll->conn), "messages.getLongPollServer", &parsed, &resterror, NULL)) {
        //TODO: error handle
        g_error ("Method messages.getLongPollServer returned %i, %s", resterror->code, resterror->message);
        return FALSE;
      }

      response = json_value_find (parsed, "response");

      temp = json_value_find (response, "key");
      g_assert (temp != NULL);
      poll->key = g_strdup (temp->u.string.ptr);

      temp = json_value_find (response, "server");
      g_assert (temp != NULL);
      poll->server = g_strdup (temp->u.string.ptr);

      temp = json_value_find (response, "ts");
      g_assert (temp != NULL);
      poll->ts = g_strdup_printf ("%li", temp->u.integer);

      json_value_free (parsed);
      failed = FALSE;

    }

    /* Set online status every 14 mins
     * https://vk.com/dev/account.setOnline */
    if (g_timer_elapsed (poll->timer, NULL) >= 840 || first_time) {
      first_time = FALSE;
      if (invoke_vk_api (DURKA_CONNECTION (poll->conn), "account.setOnline", &parsed, &resterror, NULL)) {
        //TODO: error handle
        g_error ("Method account.setOnline returned %i, %s", resterror->code, resterror->message);
        return FALSE;
      }

      response = json_value_find (parsed, "response");

      if (response->u.integer == 1)
        tp_base_connection_change_status (poll->conn, TP_CONNECTION_STATUS_CONNECTED,
                                          TP_CONNECTION_STATUS_REASON_REQUESTED);
      else
        tp_base_connection_change_status (poll->conn, TP_CONNECTION_STATUS_DISCONNECTED,
                                          TP_CONNECTION_STATUS_REASON_NETWORK_ERROR);

      json_value_free (parsed);
    }

#ifdef ENABLE_DEBUG
    g_print ("polling server... ");
#endif

    poll->session = soup_session_sync_new ();
    gchar *url = g_strdup_printf ("http://%s?act=a_check&key=%s&ts=%s&wait=25&mode=2", poll->server,
                                  poll->key, poll->ts);
    poll->msg = soup_message_new ("GET", url);
    soup_message_set_request (poll->msg, "application/json", SOUP_MEMORY_COPY, NULL, 0);
    guint status = soup_session_send_message (poll->session, poll->msg);
    if (status != SOUP_STATUS_CANCELLED) {
      g_assert (status == 200);
      g_assert (g_utf8_validate (poll->msg->response_body->data, -1, NULL));

#ifdef ENABLE_DEBUG
      g_print ("response: %s\n", poll->msg->response_body->data);
#endif

      parsed = json_parse (poll->msg->response_body->data, strlen (poll->msg->response_body->data));
      g_assert (parsed->type == json_object);

      if (json_value_find (parsed, "failed"))
        failed = TRUE;

      if (!failed) {

        g_free (poll->ts);
        poll->ts = g_strdup_printf ("%li", json_value_find (parsed, "ts")->u.integer);

        updates = json_value_find (parsed, "updates");
        if (updates->u.array.length == 0) {
          g_print ("No events. Sad..\n");
        } else {
          guint i;
          for (i = 0; i < (guint) updates->u.array.length; i++) {
            json_value *event = updates->u.array.values[i];
            event_code = event->u.array.values[0]->u.integer;
            /*
             0,$message_id,0 -- удаление сообщения с указанным local_id
             1,$message_id,$flags -- замена флагов сообщения (FLAGS:=$flags)
             2,$message_id,$mask[,$user_id] -- установка флагов сообщения (FLAGS|=$mask)
             3,$message_id,$mask[,$user_id] -- сброс флагов сообщения (FLAGS&=~$mask)
             4,$message_id,$flags,$from_id,$timestamp,$subject,$text,$attachments -- добавление нового сообщения
             8,-$user_id,0 -- друг $user_id стал онлайн
             9,-$user_id,$flags -- друг $user_id стал оффлайн ($flags равен 0, если пользователь покинул сайт (например, нажал выход) и 1, если оффлайн по таймауту (например, статус away))


             51,$chat_id,$self -- один из параметров (состав, тема) беседы $chat_id были изменены. $self - были ли изменения вызываны самим пользователем
             61,$user_id,$flags -- пользователь $user_id начал набирать текст в диалоге. событие должно приходить раз в ~5 секунд при постоянном наборе текста. $flags = 1
             62,$user_id,$chat_id -- пользователь $user_id начал набирать текст в беседе $chat_id.
             70,$user_id,$call_id -- пользователь $user_id совершил звонок имеющий идентификатор $call_id, дополнительную информацию о звонке можно получить используя метод voip.getCallInfo.*/

            switch (event_code) {
              case 0:
                g_print ("someone deleted the msg, ");
                break;
              case 1:
                g_print ("someone changed flags of the msg, ");
                break;
              case 2:
                g_print ("someone set flags of the msg, ");
                break;
              case 3:
                g_print ("someone unset flags of the msg, ");
                break;
              case 4:
                g_print ("someone added the msg, ");
                break;
              case 8:
                user_id = event->u.array.values[1]->u.integer * (-1);
                g_print ("%i is online!\n", user_id);
                break;
              case 9:
                user_id = event->u.array.values[1]->u.integer * (-1);
                g_print ("%i is offline!\n", user_id);
                break;
              case 51:
                g_print ("someone changed the chat params, ");
                break;
              case 61:
                user_id = event->u.array.values[1]->u.integer;
                g_print ("%i started to type a msg!\n", user_id);
                break;
              case 62:
                g_print ("someone started to type a msg (again), ");
                break;
              case 70:
                g_print ("someone is calling, ");
                break;
              case 101:
                g_print ("pavel durov is an idiot, ");
                break;
              default:
                g_print ("meteorite%i?, ", event_code);
            }
          }
        }
      }
      json_value_free (parsed);
    }

  }

  /* Set offline status at disconnect
   * https://vk.com/dev/account.setOffline */
  if (invoke_vk_api (DURKA_CONNECTION (poll->conn), "account.setOffline", &parsed, &resterror, NULL)) {
    //TODO: error handle
    g_error ("Method account.setOffline returned %i, %s", resterror->code, resterror->message);
    return FALSE;
  }

  response = json_value_find (parsed, "response");

  if (response->u.integer == 1)
    tp_base_connection_change_status (poll->conn, TP_CONNECTION_STATUS_DISCONNECTED,
                                      TP_CONNECTION_STATUS_REASON_REQUESTED);
  else
    tp_base_connection_change_status (poll->conn, TP_CONNECTION_STATUS_DISCONNECTED,
                                      TP_CONNECTION_STATUS_REASON_NETWORK_ERROR);

  json_value_free (parsed);

  g_thread_exit (NULL);
  return TRUE;
}

static gboolean
start_connecting (TpBaseConnection *conn,
                  GError **error)
{
  DurkaConnection *self = DURKA_CONNECTION (conn);
  TpHandleRepoIface *contact_repo = tp_base_connection_get_handles (conn, TP_HANDLE_TYPE_CONTACT);
  TpHandle self_handle;

  tp_base_connection_change_status (conn, TP_CONNECTION_STATUS_CONNECTING,
                                    TP_CONNECTION_STATUS_REASON_REQUESTED);

  self_handle = tp_handle_ensure (contact_repo, self->priv->account, NULL, error);

  if (self_handle == 0)
    return FALSE;

#ifdef ENABLE_DEBUG
  setlocale (LC_ALL, "");
#endif

  self->priv->rest = rest_proxy_new ("https://api.vk.com/method/", FALSE);

  json_value *parsed = NULL;
  GError *err = NULL;

  if (invoke_vk_api (self, "users.get", &parsed, &err, "fields", "photo_100", NULL) != 0) {
    //TODO: error handle
    g_error ("Method users.get returned code %i, %s", err->code, err->message);
  }
  json_value *response = json_value_find (parsed, "response");
#ifdef ENABLE_DEBUG
  g_print ("Hi, %s %s!\n", json_value_find (response->u.array.values[0], "first_name")->u.string.ptr,
           json_value_find (response->u.array.values[0], "last_name")->u.string.ptr);
#endif

  json_value_free (parsed);

  self->priv->data = g_slice_new (PollData);
  PollData *poll = self->priv->data;
  poll->rest = self->priv->rest;
  poll->conn = conn;
  poll->token = self->priv->token;
  poll->timer = g_timer_new ();
  poll->exit = FALSE;

  self->priv->thread = g_thread_new ("polling", (GThreadFunc) polling, self->priv->data);

  tp_base_connection_set_self_handle (conn, self_handle);

  return TRUE;
}

static void
shut_down (TpBaseConnection *conn)
{
  DurkaConnection *self = DURKA_CONNECTION (conn);
  PollData *poll = self->priv->data;
  poll->exit = TRUE;
  soup_session_cancel_message (poll->session, poll->msg, SOUP_STATUS_CANCELLED);
  g_thread_join (self->priv->thread);
  g_object_unref (self->priv->rest);
  tp_base_connection_finish_shutdown (conn);
}

static void
aliasing_fill_contact_attributes (GObject *object,
                                  const GArray *contacts,
                                  GHashTable *attributes)
{
  DurkaConnection *self = DURKA_CONNECTION (object);
  guint i;

  for (i = 0; i < contacts->len; i++) {
    TpHandle contact = g_array_index (contacts, guint, i);

    tp_contacts_mixin_set_contact_attribute (
        attributes, contact, TP_TOKEN_CONNECTION_INTERFACE_ALIASING_ALIAS,
        tp_g_value_slice_new_string (durka_contact_list_get_alias (self->priv->contact_list, contact)));
  }
}

static void
constructed (GObject *object)
{
  TpBaseConnection *base = TP_BASE_CONNECTION (object);
  void (*chain_up) (GObject *) = G_OBJECT_CLASS (durka_connection_parent_class)->constructed;

  if (chain_up != NULL)
    chain_up (object);

  tp_contacts_mixin_init (object, G_STRUCT_OFFSET (DurkaConnection, contacts_mixin));

  tp_base_connection_register_with_contacts_mixin (base);
  tp_base_contact_list_mixin_register_with_contacts_mixin (base);

  tp_contacts_mixin_add_contact_attributes_iface (object, TP_IFACE_CONNECTION_INTERFACE_ALIASING,
                                                  aliasing_fill_contact_attributes);

  tp_presence_mixin_init (object, G_STRUCT_OFFSET (DurkaConnection, presence_mixin));
  tp_presence_mixin_simple_presence_register_with_contacts_mixin (object);
}

static gboolean
status_available (GObject *object,
                  guint index_)
{
  TpBaseConnection *base = TP_BASE_CONNECTION (object);

  return tp_base_connection_check_connected (base, NULL);
}

static GHashTable *
get_contact_statuses (GObject *object,
                      const GArray *contacts,
                      GError **error)
{
  DurkaConnection *self = DURKA_CONNECTION (object);
  TpBaseConnection *base = TP_BASE_CONNECTION (object);
  guint i;
  GHashTable *result = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL,
                                              (GDestroyNotify) tp_presence_status_free);

  for (i = 0; i < contacts->len; i++) {
    TpHandle contact = g_array_index (contacts, guint, i);
    DurkaContactListPresence presence;
    GHashTable *parameters;

    /* we get our own status from the connection, and everyone else's status
     * from the contact lists */
    if (contact == tp_base_connection_get_self_handle (base)) {
      presence = (self->priv->away ? DURKA_CONTACT_LIST_PRESENCE_AWAY : DURKA_CONTACT_LIST_PRESENCE_AVAILABLE);
    } else {
      presence = durka_contact_list_get_presence (self->priv->contact_list, contact);
    }

    parameters = g_hash_table_new_full (g_str_hash, g_str_equal, NULL,
                                        (GDestroyNotify) tp_g_value_slice_free);
    g_hash_table_insert (result, GUINT_TO_POINTER (contact), tp_presence_status_new (presence, parameters));
    g_hash_table_unref (parameters);
  }

  return result;
}

static gboolean
set_own_status (GObject *object,
                const TpPresenceStatus *status,
                GError **error)
{
  DurkaConnection *self = DURKA_CONNECTION (object);
  TpBaseConnection *base = TP_BASE_CONNECTION (object);
  GHashTable *presences;

  if (status->index == DURKA_CONTACT_LIST_PRESENCE_AWAY) {
    if (self->priv->away)
      return TRUE;

    self->priv->away = TRUE;
  } else {
    if (!self->priv->away)
      return TRUE;

    self->priv->away = FALSE;
  }

  presences = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, NULL);
  g_hash_table_insert (presences, GUINT_TO_POINTER (tp_base_connection_get_self_handle (base)),
                       (gpointer) status);
  tp_presence_mixin_emit_presence_update (object, presences);
  g_hash_table_unref (presences);
  return TRUE;
}

static const gchar *interfaces_always_present[] = {
    TP_IFACE_CONNECTION_INTERFACE_ALIASING,
    TP_IFACE_CONNECTION_INTERFACE_CONTACTS,
    TP_IFACE_CONNECTION_INTERFACE_CONTACT_LIST,
    TP_IFACE_CONNECTION_INTERFACE_CONTACT_GROUPS,
    TP_IFACE_CONNECTION_INTERFACE_CONTACT_BLOCKING,
    TP_IFACE_CONNECTION_INTERFACE_PRESENCE,
    TP_IFACE_CONNECTION_INTERFACE_REQUESTS,
    TP_IFACE_CONNECTION_INTERFACE_SIMPLE_PRESENCE,
    NULL};

const gchar * const *
durka_connection_get_possible_interfaces (void)
{
  /* in this durka CM we don't have any extra interfaces that are sometimes,
   * but not always, present */
  return interfaces_always_present;
}

static GPtrArray *
get_interfaces_always_present (TpBaseConnection *base)
{
  GPtrArray *interfaces;
  guint i;

  interfaces = TP_BASE_CONNECTION_CLASS (durka_connection_parent_class)->get_interfaces_always_present (base);

  for (i = 0; interfaces_always_present[i] != NULL; i++)
    g_ptr_array_add (interfaces, (gchar *) interfaces_always_present[i]);

  return interfaces;
}

static void
durka_connection_class_init (DurkaConnectionClass *klass)
{
  TpBaseConnectionClass *base_class = (TpBaseConnectionClass *) klass;
  GObjectClass *object_class = (GObjectClass *) klass;
  GParamSpec *param_spec;

  object_class->get_property = get_property;
  object_class->set_property = set_property;
  object_class->constructed = constructed;
  object_class->finalize = finalize;
  g_type_class_add_private (klass, sizeof (DurkaConnectionPrivate));

  base_class->create_handle_repos = create_handle_repos;
  base_class->get_unique_connection_name = get_unique_connection_name;
  base_class->create_channel_managers = create_channel_managers;
  base_class->start_connecting = start_connecting;
  base_class->shut_down = shut_down;
  base_class->get_interfaces_always_present = get_interfaces_always_present;

  param_spec = g_param_spec_string ("account", "Account login", "The username of this user", NULL,
                                    G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class, PROP_ACCOUNT, param_spec);

  param_spec = g_param_spec_string ("password", "Account password",
                                    "Password to authenticate to the server with", NULL,
                                    G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class, PROP_PASSWORD, param_spec);

  param_spec = g_param_spec_string ("token", "Account token", "Token to authenticate to the server with", NULL,
                                    G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class, PROP_TOKEN, param_spec);

  tp_contacts_mixin_class_init (object_class, G_STRUCT_OFFSET (DurkaConnectionClass, contacts_mixin));

  tp_presence_mixin_class_init (object_class, G_STRUCT_OFFSET (DurkaConnectionClass, presence_mixin),
                                status_available, get_contact_statuses, set_own_status,
                                durka_contact_list_presence_statuses ());
  tp_presence_mixin_simple_presence_init_dbus_properties (object_class);

  tp_base_contact_list_mixin_class_init (base_class);
}

static void
get_alias_flags (TpSvcConnectionInterfaceAliasing *aliasing,
                 DBusGMethodInvocation *context)
{
  TpBaseConnection *base = TP_BASE_CONNECTION (aliasing);

  TP_BASE_CONNECTION_ERROR_IF_NOT_CONNECTED (base, context);
  tp_svc_connection_interface_aliasing_return_from_get_alias_flags (context,
                                                                    TP_CONNECTION_ALIAS_FLAG_USER_SET);
}

static void
get_aliases (TpSvcConnectionInterfaceAliasing *aliasing,
             const GArray *contacts,
             DBusGMethodInvocation *context)
{
  DurkaConnection *self = DURKA_CONNECTION (aliasing);
  TpBaseConnection *base = TP_BASE_CONNECTION (aliasing);
  TpHandleRepoIface *contact_repo = tp_base_connection_get_handles (base, TP_HANDLE_TYPE_CONTACT);
  GHashTable *result;
  GError *error = NULL;
  guint i;

  TP_BASE_CONNECTION_ERROR_IF_NOT_CONNECTED (base, context);

  if (!tp_handles_are_valid (contact_repo, contacts, FALSE, &error)) {
    dbus_g_method_return_error (context, error);
    g_error_free (error);
    return;
  }

  result = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, NULL);

  for (i = 0; i < contacts->len; i++) {
    TpHandle contact = g_array_index (contacts, TpHandle, i);
    const gchar *alias = durka_contact_list_get_alias (self->priv->contact_list, contact);

    g_hash_table_insert (result, GUINT_TO_POINTER (contact), (gchar *) alias);
  }

  tp_svc_connection_interface_aliasing_return_from_get_aliases (context, result);
  g_hash_table_unref (result);
}

static void
request_aliases (TpSvcConnectionInterfaceAliasing *aliasing,
                 const GArray *contacts,
                 DBusGMethodInvocation *context)
{
  DurkaConnection *self = DURKA_CONNECTION (aliasing);
  TpBaseConnection *base = TP_BASE_CONNECTION (aliasing);
  TpHandleRepoIface *contact_repo = tp_base_connection_get_handles (base, TP_HANDLE_TYPE_CONTACT);
  GPtrArray *result;
  gchar **strings;
  GError *error = NULL;
  guint i;

  TP_BASE_CONNECTION_ERROR_IF_NOT_CONNECTED (base, context);

  if (!tp_handles_are_valid (contact_repo, contacts, FALSE, &error)) {
    dbus_g_method_return_error (context, error);
    g_error_free (error);
    return;
  }

  result = g_ptr_array_sized_new (contacts->len + 1);

  for (i = 0; i < contacts->len; i++) {
    TpHandle contact = g_array_index (contacts, TpHandle, i);
    const gchar *alias = durka_contact_list_get_alias (self->priv->contact_list, contact);

    g_ptr_array_add (result, (gchar *) alias);
  }

  g_ptr_array_add (result, NULL);
  strings = (gchar **) g_ptr_array_free (result, FALSE);
  tp_svc_connection_interface_aliasing_return_from_request_aliases (context, (const gchar **) strings);
  g_free (strings);
}

static void
set_aliases (TpSvcConnectionInterfaceAliasing *aliasing,
             GHashTable *aliases,
             DBusGMethodInvocation *context)
{
  DurkaConnection *self = DURKA_CONNECTION (aliasing);
  TpBaseConnection *base = TP_BASE_CONNECTION (aliasing);
  TpHandleRepoIface *contact_repo = tp_base_connection_get_handles (base, TP_HANDLE_TYPE_CONTACT);
  GHashTableIter iter;
  gpointer key, value;

  g_hash_table_iter_init (&iter, aliases);

  while (g_hash_table_iter_next (&iter, &key, &value)) {
    GError *error = NULL;

    if (!tp_handle_is_valid (contact_repo, GPOINTER_TO_UINT (key), &error)) {
      dbus_g_method_return_error (context, error);
      g_error_free (error);
      return;
    }
  }

  g_hash_table_iter_init (&iter, aliases);

  while (g_hash_table_iter_next (&iter, &key, &value)) {
    durka_contact_list_set_alias (self->priv->contact_list, GPOINTER_TO_UINT (key), value);
  }

  tp_svc_connection_interface_aliasing_return_from_set_aliases (context);
}

static void
init_aliasing (gpointer iface,
               gpointer iface_data G_GNUC_UNUSED)
{
  TpSvcConnectionInterfaceAliasingClass *klass = iface;

#define IMPLEMENT(x) tp_svc_connection_interface_aliasing_implement_##x (\
    klass, x)
  IMPLEMENT(get_alias_flags);
  IMPLEMENT(request_aliases);
  IMPLEMENT(get_aliases);
  IMPLEMENT(set_aliases);
#undef IMPLEMENT
}

