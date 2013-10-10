#ifndef DURKA_PROTOCOL_H
#define DURKA_PROTOCOL_H

#include <glib-object.h>
#include <telepathy-glib/telepathy-glib.h>

G_BEGIN_DECLS

  typedef struct _DurkaProtocol DurkaProtocol;
  typedef struct _DurkaProtocolPrivate DurkaProtocolPrivate;
  typedef struct _DurkaProtocolClass DurkaProtocolClass;
  typedef struct _DurkaProtocolClassPrivate DurkaProtocolClassPrivate;

  struct _DurkaProtocolClass {
    TpBaseProtocolClass parent_class;

    DurkaProtocolClassPrivate *priv;
  };

  struct _DurkaProtocol {
    TpBaseProtocol parent;

    DurkaProtocolPrivate *priv;
  };

  GType
  durka_protocol_get_type (void);

#define DURKA_TYPE_PROTOCOL \
    (durka_protocol_get_type ())
#define DURKA_PROTOCOL(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
        DURKA_TYPE_PROTOCOL, \
        DurkaProtocol))
#define DURKA_PROTOCOL_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_CAST ((klass), \
        DURKA_TYPE_PROTOCOL, \
        DurkaProtocolClass))
#define DURKA_IS_PROTOCOL(obj) \
    (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
        DURKA_TYPE_PROTOCOL))
#define DURKA_IS_PROTOCOL_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_TYPE ((klass), \
        DURKA_TYPE_PROTOCOL))
#define DURKA_PROTOCOL_GET_CLASS(obj) \
    (G_TYPE_INSTANCE_GET_CLASS ((obj), \
        DURKA_TYPE_PROTOCOL, \
        DurkaProtocolClass))

  gboolean
  durka_protocol_check_contact_id (const gchar *id,
                                   gchar **normal,
                                   GError **error);

G_END_DECLS

#endif

