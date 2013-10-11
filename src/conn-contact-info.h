/*
 * conn-contact-info.h
 *
 *  Created on: 11 окт. 2013 г.
 *      Author: morse
 */

#ifndef CONN_CONTACT_INFO_H_
#define CONN_CONTACT_INFO_H_

#include "conn.h"

G_BEGIN_DECLS

void conn_contact_info_class_init (DurkaConnectionClass *klass);
void conn_contact_info_init (DurkaConnection *conn);
void conn_contact_info_finalize (DurkaConnection *conn);
void conn_contact_info_iface_init (gpointer g_iface, gpointer iface_data);

extern TpDBusPropertiesMixinPropImpl *conn_contact_info_properties;
void conn_contact_info_properties_getter (GObject *object, GQuark interface,
    GQuark name, GValue *value, gpointer getter_data);

G_END_DECLS

#endif /* CONN_CONTACT_INFO_H_ */
