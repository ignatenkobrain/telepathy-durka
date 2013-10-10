#ifndef __DURKA_CONN_H__
#define __DURKA_CONN_H__

#include <glib-object.h>
#include <telepathy-glib/telepathy-glib.h>
#include <json.h>

G_BEGIN_DECLS

  typedef struct _DurkaConnection DurkaConnection;
  typedef struct _DurkaConnectionClass DurkaConnectionClass;
  typedef struct _DurkaConnectionPrivate DurkaConnectionPrivate;

  struct _DurkaConnectionClass {
    TpBaseConnectionClass parent_class;
    TpPresenceMixinClass presence_mixin;
    TpContactsMixinClass contacts_mixin;
  };

  struct _DurkaConnection {
    TpBaseConnection parent;
    TpPresenceMixin presence_mixin;
    TpContactsMixin contacts_mixin;

    DurkaConnectionPrivate *priv;
  };

  GType
  durka_connection_get_type (void);

#define DURKA_TYPE_CONNECTION \
  (durka_connection_get_type ())
#define DURKA_CONNECTION(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), DURKA_TYPE_CONNECTION, \
                              DurkaConnection))
#define DURKA_CONNECTION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass), DURKA_TYPE_CONNECTION, \
                           DurkaConnectionClass))
#define DURKA_IS_CONNECTION(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj), DURKA_TYPE_CONNECTION))
#define DURKA_IS_CONNECTION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass), DURKA_TYPE_CONNECTION))
#define DURKA_CONNECTION_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), DURKA_TYPE_CONNECTION, \
                              DurkaConnectionClass))

  gint
  invoke_vk_api (DurkaConnection *self,
                 const gchar *method,
                 json_value **response,
                 GError **error,
                 ...);

  gchar *
  durka_normalize_contact (TpHandleRepoIface *repo,
                           const gchar *id,
                           gpointer context,
                           GError **error);

  const gchar * const *
  durka_connection_get_possible_interfaces (void);

G_END_DECLS

#endif

