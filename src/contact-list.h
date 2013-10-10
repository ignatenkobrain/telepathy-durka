#ifndef __DURKA_CONTACT_LIST_H__
#define __DURKA_CONTACT_LIST_H__

#include <glib-object.h>

#include <telepathy-glib/telepathy-glib.h>

G_BEGIN_DECLS

  typedef struct _DurkaContactList DurkaContactList;
  typedef struct _DurkaContactListClass DurkaContactListClass;
  typedef struct _DurkaContactListPrivate DurkaContactListPrivate;

  struct _DurkaContactListClass {
    TpBaseContactListClass parent_class;
  };

  struct _DurkaContactList {
    TpBaseContactList parent;

    DurkaContactListPrivate *priv;
  };

  GType
  durka_contact_list_get_type (void);

#define DURKA_TYPE_CONTACT_LIST \
  (durka_contact_list_get_type ())
#define DURKA_CONTACT_LIST(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), DURKA_TYPE_CONTACT_LIST, \
                              DurkaContactList))
#define DURKA_CONTACT_LIST_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass), DURKA_TYPE_CONTACT_LIST, \
                           DurkaContactListClass))
#define DURKA_IS_CONTACT_LIST(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj), DURKA_TYPE_CONTACT_LIST))
#define DURKA_IS_CONTACT_LIST_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass), DURKA_TYPE_CONTACT_LIST))
#define DURKA_CONTACT_LIST_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), DURKA_TYPE_CONTACT_LIST, \
                              DurkaContactListClass))

  /* this enum must be kept in sync with the array _statuses in
   * contact-list.c */
  typedef enum {
    DURKA_CONTACT_LIST_PRESENCE_OFFLINE = 0,
    DURKA_CONTACT_LIST_PRESENCE_UNKNOWN,
    DURKA_CONTACT_LIST_PRESENCE_ERROR,
    DURKA_CONTACT_LIST_PRESENCE_AWAY,
    DURKA_CONTACT_LIST_PRESENCE_AVAILABLE
  } DurkaContactListPresence;

  const TpPresenceStatusSpec *
  durka_contact_list_presence_statuses (void);

  DurkaContactListPresence
  durka_contact_list_get_presence (DurkaContactList *self,
                                   TpHandle contact);
  const gchar *
  durka_contact_list_get_alias (DurkaContactList *self,
                                TpHandle contact);
  void
  durka_contact_list_set_alias (DurkaContactList *self,
                                TpHandle contact,
                                const gchar *alias);

G_END_DECLS

#endif

