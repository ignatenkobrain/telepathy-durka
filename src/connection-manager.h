#ifndef __DURKA_CONNECTION_MANAGER_H__
#define __DURKA_CONNECTION_MANAGER_H__

#include <glib-object.h>
#include <telepathy-glib/telepathy-glib.h>

G_BEGIN_DECLS

  typedef struct _DurkaConnectionManager DurkaConnectionManager;
  typedef struct _DurkaConnectionManagerClass DurkaConnectionManagerClass;
  typedef struct _DurkaConnectionManagerPrivate DurkaConnectionManagerPrivate;

  struct _DurkaConnectionManagerClass {
    TpBaseConnectionManagerClass parent_class;
  };

  struct _DurkaConnectionManager {
    TpBaseConnectionManager parent;

    DurkaConnectionManagerPrivate *priv;
  };

  GType
  durka_connection_manager_get_type (void);

#define DURKA_TYPE_CONNECTION_MANAGER \
  (durka_connection_manager_get_type ())
#define DURKA_CONNECTION_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), \
                              DURKA_TYPE_CONNECTION_MANAGER, \
                              DurkaConnectionManager))
#define DURKA_CONNECTION_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass), \
                           DURKA_TYPE_CONNECTION_MANAGER, \
                           DurkaConnectionManagerClass))
#define DURKA_IS_CONNECTION_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj), \
                              DURKA_TYPE_CONNECTION_MANAGER))
#define DURKA_IS_CONNECTION_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass), \
                           DURKA_TYPE_CONNECTION_MANAGER))
#define DURKA_CONNECTION_MANAGER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                              DURKA_TYPE_CONNECTION_MANAGER, \
                              DurkaConnectionManagerClass))

G_END_DECLS

#endif

