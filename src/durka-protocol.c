/* durka-protocol.c
 *
 * Copyright (C) 2015 Igor Gnatenko <i.gnatenko.brain@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "durka-connection.h"
#include "durka-contact-list.h"
#include "durka-protocol.h"

G_DEFINE_TYPE (DurkaProtocol, durka_protocol, TP_TYPE_BASE_PROTOCOL)

gboolean
durka_protocol_check_contact_id (const gchar  *id,
                                 gchar       **normal,
                                 GError      **error)
{
  g_return_val_if_fail (id != NULL, FALSE);

  if (id[0] == '\0') {
    g_set_error (error, TP_ERROR, TP_ERROR_INVALID_HANDLE,
                 "ID must not be empty");
    return FALSE;
  }

  if (normal != NULL)
    *normal = g_utf8_normalize (id, -1, G_NORMALIZE_ALL_COMPOSE);

  return TRUE;
}

static gboolean
account_param_filter (const TpCMParamSpec  *paramspec,
                      GValue               *value,
                      GError              **error)
{
  const gchar *id = g_value_get_string (value);

  return durka_protocol_check_contact_id (id, NULL, error);
}

static const TpCMParamSpec durka_params[] = {
  { "account", "s", G_TYPE_STRING,
    TP_CONN_MGR_PARAM_FLAG_REQUIRED | TP_CONN_MGR_PARAM_FLAG_REGISTER,
    NULL,
    0,
    account_param_filter,
    NULL,
    NULL },
  { "password", "s", G_TYPE_STRING,
    TP_CONN_MGR_PARAM_FLAG_REQUIRED | TP_CONN_MGR_PARAM_FLAG_REGISTER | TP_CONN_MGR_PARAM_FLAG_SECRET,
    NULL,
    0,
    NULL,
    NULL,
    NULL },
  { "token", "s", G_TYPE_STRING,
    TP_CONN_MGR_PARAM_FLAG_SECRET,
    NULL,
    0,
    NULL,
    NULL,
    NULL },
  { NULL }
};

static const TpCMParamSpec *
get_parameters (TpBaseProtocol *self)
{
  return durka_params;
}

static TpBaseConnection *
new_connection (TpBaseProtocol  *protocol,
                GHashTable      *asv,
                GError         **error)
{
  DurkaConnection *conn;
  const gchar *account;
  const gchar *password;
  const gchar *token;

  account = tp_asv_get_string (asv, "account");
  password = tp_asv_get_string (asv, "password");
  token = tp_asv_get_string (asv, "token");
  /* telepathy-glib checked this for us */
  g_assert (account != NULL);

  conn = DURKA_CONNECTION (
      g_object_new (DURKA_TYPE_CONNECTION,
                    "account", account,
                    "protocol", tp_base_protocol_get_name (protocol),
                    "password", password,
                    "token", token,
                    NULL));

  return (TpBaseConnection *) conn;
}

static gchar *
normalize_contact (TpBaseProtocol  *self G_GNUC_UNUSED,
                   const gchar     *contact,
                   GError         **error)
{
  gchar *normal;

  if (durka_protocol_check_contact_id (contact, &normal, error))
    return normal;
  else
    return NULL;
}

static gchar *
identify_account (TpBaseProtocol  *self G_GNUC_UNUSED,
                  GHashTable      *asv,
                  GError         **error)
{
  const gchar *account = tp_asv_get_string (asv, "account");

  if (account != NULL)
    return normalize_contact (self, account, error);

  g_set_error (error, TP_ERROR, TP_ERROR_INVALID_ARGUMENT,
               "'account' parameter not given");
  return NULL;
}

static void
get_connection_details (TpBaseProtocol  *self G_GNUC_UNUSED,
                        GStrv           *connection_interfaces,
                        GType          **channel_managers,
                        gchar          **icon_name,
                        gchar          **english_name,
                        gchar          **vcard_field)
{
  if (connection_interfaces != NULL) {
    *connection_interfaces = g_strdupv ((GStrv) durka_connection_get_implemented_interfaces ());
  }

  if (channel_managers != NULL) {
    GType types[] = {DURKA_TYPE_CONTACT_LIST, G_TYPE_INVALID};

    *channel_managers = g_memdup (types, sizeof (types));
  }

  if (icon_name != NULL)
    *icon_name = g_strdup ("vk");

  if (english_name != NULL)
    *english_name = g_strdup ("Vk with a contact list");

  if (vcard_field != NULL)
    *vcard_field = g_strdup ("x-telepathy-vk");
}

static void
durka_protocol_class_init (DurkaProtocolClass *klass)
{
  TpBaseProtocolClass *base_class = (TpBaseProtocolClass *) klass;

  base_class->get_parameters = get_parameters;
  base_class->new_connection = new_connection;

  base_class->normalize_contact = normalize_contact;
  base_class->identify_account = identify_account;
  base_class->get_connection_details = get_connection_details;
}

static void
durka_protocol_init (DurkaProtocol *self)
{
}
