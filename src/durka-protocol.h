/* durka-protocol.h
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

#ifndef DURKA_PROTOCOL_H
#define DURKA_PROTOCOL_H

#include <glib-object.h>
#include <telepathy-glib/telepathy-glib.h>

G_BEGIN_DECLS

#define DURKA_TYPE_PROTOCOL            (durka_protocol_get_type())
#define DURKA_PROTOCOL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DURKA_TYPE_PROTOCOL, DurkaProtocol))
#define DURKA_PROTOCOL_CONST(obj)      (G_TYPE_CHECK_INSTANCE_CAST ((obj), DURKA_TYPE_PROTOCOL, DurkaProtocol const))
#define DURKA_PROTOCOL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  DURKA_TYPE_PROTOCOL, DurkaProtocolClass))
#define DURKA_IS_PROTOCOL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DURKA_TYPE_PROTOCOL))
#define DURKA_IS_PROTOCOL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  DURKA_TYPE_PROTOCOL))
#define DURKA_PROTOCOL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  DURKA_TYPE_PROTOCOL, DurkaProtocolClass))

typedef struct _DurkaProtocol             DurkaProtocol;
typedef struct _DurkaProtocolClass        DurkaProtocolClass;
typedef struct _DurkaProtocolPrivate      DurkaProtocolPrivate;
typedef struct _DurkaProtocolClassPrivate DurkaProtocolClassPrivate;

struct _DurkaProtocol
{
  TpBaseProtocol parent;

  /*< private >*/
  DurkaProtocolPrivate *priv;
};

struct _DurkaProtocolClass
{
  TpBaseProtocolClass parent_class;

  DurkaProtocolClassPrivate *priv;
};

GType              durka_protocol_get_type (void);

gboolean
durka_protocol_check_contact_id (const gchar  *id,
                                 gchar       **normal,
                                 GError      **error);

G_END_DECLS

#endif /* DURKA_PROTOCOL_H */
