/* durka-connection-manager.c
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
#include "durka-connection-manager.h"
#include "durka-protocol.h"

struct _DurkaConnectionManagerPrivate
{
  int dummy;
};

G_DEFINE_TYPE_WITH_PRIVATE (DurkaConnectionManager, durka_connection_manager, TP_TYPE_BASE_CONNECTION_MANAGER)

static void
durka_connection_manager_constructed (GObject *object)
{
  DurkaConnectionManager *self = DURKA_CONNECTION_MANAGER (object);
  TpBaseConnectionManager *base = (TpBaseConnectionManager *) self;
  void (*constructed) (GObject *) =
    ((GObjectClass *) durka_connection_manager_parent_class)->constructed;
  TpBaseProtocol *protocol;

  if (constructed != NULL)
    constructed (object);

  protocol = g_object_new (DURKA_TYPE_PROTOCOL, "name", "vk", NULL);
  tp_base_connection_manager_add_protocol (base, protocol);
  g_object_unref (protocol);
}

static void
durka_connection_manager_class_init (DurkaConnectionManagerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  TpBaseConnectionManagerClass *base_class = (TpBaseConnectionManagerClass *) klass;

  object_class->constructed = durka_connection_manager_constructed;
  base_class->cm_dbus_name = "durka";
}

static void
durka_connection_manager_init (DurkaConnectionManager *self)
{
  self->priv = durka_connection_manager_get_instance_private (self);
}
