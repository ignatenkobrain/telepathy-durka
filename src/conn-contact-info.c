/*
 * conn-contact-info.c
 *
 *  Created on: 11 окт. 2013 г.
 *      Author: morse
 */

#include "config.h"

#include "conn-contact-info.h"

/**
 * durka_connection_get_contact_info
 *
 * Implements D-Bus method GetContactInfo
 * on interface org.freedesktop.Telepathy.Connection.Interface.ContactInfo
 *
 * @context: The D-Bus invocation context to use to return values
 *           or throw an error.
 */
static void
durka_connection_get_contact_info (TpSvcConnectionInterfaceContactInfo *iface,
                                   const GArray *contacts,
                                   DBusGMethodInvocation *context)
{
  DurkaConnection *self = DURKA_CONNECTION(iface);
  TpBaseConnection *base = (TpBaseConnection *) self;
  TpHandleRepoIface *contacts_repo = tp_base_connection_get_handles (base, TP_HANDLE_TYPE_CONTACT);
  GError *error = NULL;
  guint i;
  GHashTable *ret;

  TP_BASE_CONNECTION_ERROR_IF_NOT_CONNECTED(TP_BASE_CONNECTION (iface), context);

  if (!tp_handles_are_valid (contacts_repo, contacts, FALSE, &error)) {
    dbus_g_method_return_error (context, error);
    g_error_free (error);
    return;
  }

  ret = dbus_g_type_specialized_construct (TP_HASH_TYPE_CONTACT_INFO_MAP);

  for (i = 0; i < contacts->len; i++) {
    TpHandle contact = g_array_index(contacts, TpHandle, i);
    GPtrArray *contact_info = dbus_g_type_specialized_construct (TP_ARRAY_TYPE_CONTACT_INFO_FIELD_LIST);
    // TODO: actual filling of the contact info
    g_hash_table_insert (ret, GUINT_TO_POINTER(contact), contact_info);
  }

  tp_svc_connection_interface_contact_info_return_from_get_contact_info (context, ret);

  g_boxed_free (TP_HASH_TYPE_CONTACT_INFO_MAP, ret);
}

/**
 * durka_connection_refresh_contact_info
 *
 * Implements D-Bus method RefreshContactInfo
 * on interface org.freedesktop.Telepathy.Connection.Interface.ContactInfo
 *
 * @context: The D-Bus invocation context to use to return values
 *           or throw an error.
 */
static void
durka_connection_refresh_contact_info (TpSvcConnectionInterfaceContactInfo *iface,
                                       const GArray *contacts,
                                       DBusGMethodInvocation *context)
{
  DurkaConnection *self = DURKA_CONNECTION(iface);
  TpBaseConnection *base = (TpBaseConnection *) self;
  TpHandleRepoIface *contacts_repo = tp_base_connection_get_handles (base, TP_HANDLE_TYPE_CONTACT);
  GError *error = NULL;
  guint i;

  TP_BASE_CONNECTION_ERROR_IF_NOT_CONNECTED(TP_BASE_CONNECTION (iface), context);

  if (!tp_handles_are_valid (contacts_repo, contacts, FALSE, &error)) {
    dbus_g_method_return_error (context, error);
    g_error_free (error);
    return;
  }

  for (i = 0; i < contacts->len; i++) {
    // TODO: actual queuing of the contact refreshing
  }

  tp_svc_connection_interface_contact_info_return_from_refresh_contact_info (context);
}

/**
 * durka_connection_request_contact_info
 *
 * Implements D-Bus method RequestContactInfo
 * on interface org.freedesktop.Telepathy.Connection.Interface.ContactInfo
 *
 * @context: The D-Bus invocation context to use to return values
 *           or throw an error.
 */
static void
durka_connection_request_contact_info (TpSvcConnectionInterfaceContactInfo *iface,
                                       guint contact,
                                       DBusGMethodInvocation *context)
{
  DurkaConnection *self = DURKA_CONNECTION (iface);
  TpBaseConnection *base = (TpBaseConnection *) self;
  TpHandleRepoIface *contact_handles = tp_base_connection_get_handles (base, TP_HANDLE_TYPE_CONTACT);
  GError *error = NULL;

  TP_BASE_CONNECTION_ERROR_IF_NOT_CONNECTED(base, context);

  if (!tp_handle_is_valid (contact_handles, contact, &error)) {
    dbus_g_method_return_error (context, error);
    g_error_free (error);
    return;
  }

  GPtrArray *contact_info = dbus_g_type_specialized_construct (TP_ARRAY_TYPE_CONTACT_INFO_FIELD_LIST);
  //TODO: do something
  tp_svc_connection_interface_contact_info_return_from_request_contact_info (context, contact_info);

  g_boxed_free (TP_ARRAY_TYPE_CONTACT_INFO_FIELD_LIST, contact_info);
}

static void
conn_contact_info_fill_contact_attributes (GObject *obj,
                                           const GArray *contacts,
                                           GHashTable *attributes_hash)
{
  guint i;
  DurkaConnection *self = DURKA_CONNECTION(obj);

  for (i = 0; i < contacts->len; i++) {
    TpHandle contact = g_array_index(contacts, TpHandle, i);
    GPtrArray *contact_info = dbus_g_type_specialized_construct (TP_ARRAY_TYPE_CONTACT_INFO_FIELD_LIST);

    //TODO: do something

    GValue *val = tp_g_value_slice_new_take_boxed (TP_ARRAY_TYPE_CONTACT_INFO_FIELD_LIST, contact_info);
    tp_contacts_mixin_set_contact_attribute (attributes_hash, contact,
    TP_IFACE_CONNECTION_INTERFACE_CONTACT_INFO"/info",
                                             val);

  }
}

/**
 * durka_connection_set_contact_info
 *
 * Implements D-Bus method SetContactInfo
 * on interface org.freedesktop.Telepathy.Connection.Interface.ContactInfo
 *
 * @context: The D-Bus invocation context to use to return values
 *           or throw an error.
 */
static void
durka_connection_set_contact_info (TpSvcConnectionInterfaceContactInfo *iface,
                                    const GPtrArray *contact_info,
                                    DBusGMethodInvocation *context)
{
  DurkaConnection *self = DURKA_CONNECTION(iface);
  TpBaseConnection *base = (TpBaseConnection *) self;
  GList *edits = NULL;
  guint i;
  GError *error = NULL;

  TP_BASE_CONNECTION_ERROR_IF_NOT_CONNECTED(base, context);

  for (i = 0; i < contact_info->len; i++) {
    GValueArray *structure = g_ptr_array_index(contact_info, i);
    guint n_field_values = 0;
    const gchar *field_name;
    const gchar * const *field_params;
    const gchar * const *field_values;

    field_name = g_value_get_string (structure->values + 0);
    field_params = g_value_get_boxed (structure->values + 1);
    field_values = g_value_get_boxed (structure->values + 2);

    if (field_values != NULL)
      n_field_values = g_strv_length ((gchar **) field_values);

    //TODO: actually fill in info
  }

  //dbus_g_method_return_error (context, &error);

  tp_svc_connection_interface_contact_info_return_from_set_contact_info (context);

}
void
conn_contact_info_class_init (DurkaConnectionClass *klass)
{

}

void
conn_contact_info_init (DurkaConnection *conn)
{
  tp_contacts_mixin_add_contact_attributes_iface (G_OBJECT (conn),
    TP_IFACE_CONNECTION_INTERFACE_CONTACT_INFO,
    conn_contact_info_fill_contact_attributes);

}

void
conn_contact_info_finalize (DurkaConnection *conn)
{
}

void
conn_contact_info_iface_init (gpointer g_iface,
                              gpointer iface_data)
{
  TpSvcConnectionInterfaceContactInfoClass *klass = g_iface;

#define IMPLEMENT(x) tp_svc_connection_interface_contact_info_implement_##x (\
    klass, durka_connection_##x)
  IMPLEMENT(get_contact_info);
  IMPLEMENT(refresh_contact_info);
  IMPLEMENT(request_contact_info);
  IMPLEMENT(set_contact_info);
#undef IMPLEMENT
}

static TpDBusPropertiesMixinPropImpl props[] = {
      { "ContactInfoFlags", GUINT_TO_POINTER (TP_CONTACT_INFO_FLAG_CAN_SET),
        NULL },
      { "SupportedFields", NULL, NULL },
      { NULL }
};
TpDBusPropertiesMixinPropImpl *conn_contact_info_properties = props;

void
conn_contact_info_properties_getter (GObject *object,
                                     GQuark interface,
                                     GQuark name,
                                     GValue *value,
                                     gpointer getter_data)
{
  DurkaConnection *conn = DURKA_CONNECTION (object);
  GQuark q_supported_fields = g_quark_from_static_string (
      "SupportedFields");

  if (name == q_supported_fields)
    {
      g_value_set_boxed (value, NULL);
    }
  else
    {
      g_value_set_uint (value, GPOINTER_TO_UINT (getter_data));
    }
}
