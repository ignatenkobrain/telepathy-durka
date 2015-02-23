#include <telepathy-glib/telepathy-glib.h>

#include "config.h"
#include "connection-manager.h"

static TpBaseConnectionManager *
construct_cm (void)
{
  return (TpBaseConnectionManager *) g_object_new (
      DURKA_TYPE_CONNECTION_MANAGER, NULL);
}

int
main (int    argc,
      char **argv)
{

#ifdef ENABLE_DEBUG
  tp_debug_divert_messages (g_getenv ("DURKA_CM_LOGFILE"));
  tp_debug_set_flags (g_getenv ("DURKA_DEBUG"));

  if (g_getenv ("DURKA_TIMING") != NULL)
    g_log_set_default_handler (tp_debug_timestamped_log_handler, NULL);

  if (g_getenv ("DURKA_PERSIST") != NULL)
    tp_debug_set_persistent (TRUE);
#endif

  return tp_run_connection_manager ("telepathy-durka", VERSION, construct_cm, argc, argv);

  return 0;
}
