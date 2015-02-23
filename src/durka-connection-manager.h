/* durka-connection-manager.h
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

#ifndef DURKA_CONNECTION_MANAGER_H
#define DURKA_CONNECTION_MANAGER_H

#include <glib-object.h>
#include <telepathy-glib/telepathy-glib.h>

G_BEGIN_DECLS

#define DURKA_TYPE_CONNECTION_MANAGER            (durka_connection_manager_get_type())
#define DURKA_CONNECTION_MANAGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DURKA_TYPE_CONNECTION_MANAGER, DurkaConnectionManager))
#define DURKA_CONNECTION_MANAGER_CONST(obj)      (G_TYPE_CHECK_INSTANCE_CAST ((obj), DURKA_TYPE_CONNECTION_MANAGER, DurkaConnectionManager const))
#define DURKA_CONNECTION_MANAGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  DURKA_TYPE_CONNECTION_MANAGER, DurkaConnectionManagerClass))
#define DURKA_IS_CONNECTION_MANAGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DURKA_TYPE_CONNECTION_MANAGER))
#define DURKA_IS_CONNECTION_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  DURKA_TYPE_CONNECTION_MANAGER))
#define DURKA_CONNECTION_MANAGER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  DURKA_TYPE_CONNECTION_MANAGER, DurkaConnectionManagerClass))

typedef struct _DurkaConnectionManager        DurkaConnectionManager;
typedef struct _DurkaConnectionManagerClass   DurkaConnectionManagerClass;
typedef struct _DurkaConnectionManagerPrivate DurkaConnectionManagerPrivate;

struct _DurkaConnectionManager
{
  TpBaseConnectionManager parent;

  /*< private >*/
  DurkaConnectionManagerPrivate *priv;
};

struct _DurkaConnectionManagerClass
{
  TpBaseConnectionManagerClass parent_class;
};

GType                       durka_connection_manager_get_type (void);

G_END_DECLS

#endif /* DURKA_CONNECTION_MANAGER_H */
