#ifndef __DURKA_JSON_H__
#define __DURKA_JSON_H__

#include <json.h>
#include <glib-object.h>

json_value *
json_value_find (const json_value *parent,
                 const gchar *name);

#endif

