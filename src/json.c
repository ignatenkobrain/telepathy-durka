#include "json.h"
#include <string.h>

json_value *
json_value_find (const json_value *parent,
                 const gchar *name)
{
  guint i;

  if ((parent == NULL) || (parent->type != json_object))
    return NULL;

  for (i = 0; i < (guint) parent->u.object.length; i++)
    if (g_strcmp0 (parent->u.object.values[i].name, name) == 0)
      return parent->u.object.values[i].value;

  return NULL;
}
