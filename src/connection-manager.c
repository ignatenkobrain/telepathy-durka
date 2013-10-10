#include "config.h"

#include "connection-manager.h"

#include <dbus/dbus-protocol.h>
#include <dbus/dbus-glib.h>

#include <telepathy-glib/telepathy-glib.h>

#include "conn.h"
#include "protocol.h"

G_DEFINE_TYPE (DurkaConnectionManager,
    durka_connection_manager,
    TP_TYPE_BASE_CONNECTION_MANAGER)

struct _DurkaConnectionManagerPrivaten {
  int dummy;
};

static void
durka_connection_manager_init (DurkaConnectionManager *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, DURKA_TYPE_CONNECTION_MANAGER,
                                           DurkaConnectionManagerPrivate);
}

static void
durka_connection_manager_constructed (GObject *object)
{
  DurkaConnectionManager *self = DURKA_CONNECTION_MANAGER (object);
  TpBaseConnectionManager *base = (TpBaseConnectionManager *) self;
  void
  (*constructed) (GObject *) =
  ((GObjectClass *) durka_connection_manager_parent_class)->constructed;
  TpBaseProtocol *protocol;

  if (constructed != NULL)
    constructed (object);

  protocol = g_object_new (DURKA_TYPE_PROTOCOL, "name", "vk",
  NULL);
  tp_base_connection_manager_add_protocol (base, protocol);
  g_object_unref (protocol);
}

static void
durka_connection_manager_class_init (DurkaConnectionManagerClass *klass)
{
  GObjectClass *object_class = (GObjectClass *) klass;
  TpBaseConnectionManagerClass *base_class = (TpBaseConnectionManagerClass *) klass;

  g_type_class_add_private (klass, sizeof (DurkaConnectionManagerPrivate));

  object_class->constructed = durka_connection_manager_constructed;
  base_class->cm_dbus_name = "durka";
}

