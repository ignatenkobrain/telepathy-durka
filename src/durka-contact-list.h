/* durka-contact-list.h
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

#ifndef DURKA_CONTACT_LIST_H
#define DURKA_CONTACT_LIST_H

#include <glib-object.h>
#include <telepathy-glib/telepathy-glib.h>

G_BEGIN_DECLS

#define DURKA_TYPE_CONTACT_LIST            (durka_contact_list_get_type())
#define DURKA_CONTACT_LIST(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DURKA_TYPE_CONTACT_LIST, DurkaContactList))
#define DURKA_CONTACT_LIST_CONST(obj)      (G_TYPE_CHECK_INSTANCE_CAST ((obj), DURKA_TYPE_CONTACT_LIST, DurkaContactList const))
#define DURKA_CONTACT_LIST_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  DURKA_TYPE_CONTACT_LIST, DurkaContactListClass))
#define DURKA_IS_CONTACT_LIST(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DURKA_TYPE_CONTACT_LIST))
#define DURKA_IS_CONTACT_LIST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  DURKA_TYPE_CONTACT_LIST))
#define DURKA_CONTACT_LIST_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  DURKA_TYPE_CONTACT_LIST, DurkaContactListClass))

typedef struct _DurkaContactList        DurkaContactList;
typedef struct _DurkaContactListClass   DurkaContactListClass;
typedef struct _DurkaContactListPrivate DurkaContactListPrivate;

struct _DurkaContactList
{
  TpBaseContactList parent;

  /*< private >*/
  DurkaContactListPrivate *priv;
};

struct _DurkaContactListClass
{
  TpBaseContactListClass parent_class;
};

GType                 durka_contact_list_get_type (void);

/* this enum must be kept in sync with the array _statuses in
 * contact-list.c */
typedef enum {
  DURKA_CONTACT_LIST_PRESENCE_OFFLINE,
  DURKA_CONTACT_LIST_PRESENCE_UNKNOWN,
  DURKA_CONTACT_LIST_PRESENCE_ERROR,
  DURKA_CONTACT_LIST_PRESENCE_AWAY,
  DURKA_CONTACT_LIST_PRESENCE_AVAILABLE
} DurkaContactListPresence;

const TpPresenceStatusSpec *
durka_contact_list_presence_statuses (void);

DurkaContactListPresence
durka_contact_list_get_presence (DurkaContactList *self,
                                 TpHandle          contact);
const gchar *
durka_contact_list_get_alias (DurkaContactList *self,
                              TpHandle          contact);
void
durka_contact_list_set_alias (DurkaContactList *self,
                              TpHandle          contact,
                              const gchar      *alias);

G_END_DECLS

#endif /* DURKA_CONTACT_LIST_H */
