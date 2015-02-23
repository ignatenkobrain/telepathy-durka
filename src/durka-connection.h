/* durka-connection.h
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

#ifndef DURKA_CONNECTION_H
#define DURKA_CONNECTION_H

#include <glib-object.h>
#include <telepathy-glib/telepathy-glib.h>
#include <json-glib/json-glib.h>

G_BEGIN_DECLS

#define DURKA_TYPE_CONNECTION            (durka_connection_get_type())
#define DURKA_CONNECTION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DURKA_TYPE_CONNECTION, DurkaConnection))
#define DURKA_CONNECTION_CONST(obj)      (G_TYPE_CHECK_INSTANCE_CAST ((obj), DURKA_TYPE_CONNECTION, DurkaConnection const))
#define DURKA_CONNECTION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  DURKA_TYPE_CONNECTION, DurkaConnectionClass))
#define DURKA_IS_CONNECTION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DURKA_TYPE_CONNECTION))
#define DURKA_IS_CONNECTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  DURKA_TYPE_CONNECTION))
#define DURKA_CONNECTION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  DURKA_TYPE_CONNECTION, DurkaConnectionClass))

typedef struct _DurkaConnection        DurkaConnection;
typedef struct _DurkaConnectionClass   DurkaConnectionClass;
typedef struct _DurkaConnectionPrivate DurkaConnectionPrivate;

struct _DurkaConnection
{
  TpBaseConnection parent;
  TpPresenceMixin presence_mixin;
  TpContactsMixin contacts_mixin;

  /*< private >*/
  DurkaConnectionPrivate *priv;
};

struct _DurkaConnectionClass
{
  TpBaseConnectionClass parent_class;
  TpDBusPropertiesMixinClass properties_mixin;
  TpPresenceMixinClass presence_mixin;
  TpContactsMixinClass contacts_mixin;
};

GType                durka_connection_get_type (void);

gint
invoke_vk_api (DurkaConnection  *self,
               const gchar      *method,
               JsonReader      **response,
               GError          **error,
               ...);

gchar *
durka_normalize_contact (TpHandleRepoIface  *repo,
                         const gchar        *id,
                         gpointer            context,
                         GError            **error);

const gchar * const *
durka_connection_get_implemented_interfaces (void);

G_END_DECLS

#endif /* DURKA_CONNECTION_H */
