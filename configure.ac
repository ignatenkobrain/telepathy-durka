AC_PREREQ([2.60])

# Making releases:
#   set the new version number:
#     odd minor -> development series
#     even minor -> stable series
#     increment micro for each release within a series
#   set nano_version to 0
#   make the release, tag it
#   set nano_version to 1

m4_define([durka_major_version], [0])
m4_define([durka_minor_version], [0])
m4_define([durka_micro_version], [0])
m4_define([durka_nano_version], [1])

m4_define([durka_base_version],
          [durka_major_version.durka_minor_version.durka_micro_version])
m4_define([durka_version],
          [m4_if(durka_nano_version, 0,
                 [durka_base_version],
                 [durka_base_version].[durka_nano_version])])

AC_INIT([telepathy-durka], [durka_version],
        [https://github.com/ignatenkobrain/telepathy-durka/issues/new])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

# Enable silent build rules by default, requires at least
# Automake-1.11. Disable by either passing --disable-silent-rules to
# configure or passing V=1 to make
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AM_INIT_AUTOMAKE([1.9 -Wno-portability subdir-objects])

AM_PROG_LIBTOOL
AM_CONFIG_HEADER(config.h)
AC_PROG_MKDIR_P

dnl check for tools
AC_PROG_CC
AC_PROG_CC_STDC
AM_PROG_AS

AS_IF([test durka_nano_version = 0],
[ # version x.y.z - "official release",
  # disable extra checks by default
  AC_ARG_ENABLE([fatal-warnings],
    [AC_HELP_STRING([--enable-fatal-warnings],
                    [make various warnings fatal])],
    [],
    [enable_fatal_warnings=no])
],
[ # version x.y.z.1 - development snapshot,
  # enable extra checks by default
  AC_ARG_ENABLE([fatal-warnings],
    [AC_HELP_STRING([--disable-fatal-warnings],
                    [make various warnings non-fatal])],
    [],
    #[enable_fatal_warnings=yes])
    [enable_fatal_warnings=no])
  AC_DEFINE_UNQUOTED([ENABLE_DEBUG], [], [Enable debug messages])
])

TP_COMPILER_WARNINGS([ERROR_CFLAGS], [test "x$enable_fatal_warnings" = xyes],
  [all \
   extra \
   declaration-after-statement \
   shadow \
   missing-prototypes \
   nested-externs \
   pointer-arith \
   sign-compare \
   strict-prototypes \
   format-security \
   init-self],
  [missing-field-initializers \
   unused-parameter])
AC_SUBST([ERROR_CFLAGS])

# these aren't really error flags but they serve a similar purpose for us -
# making the toolchain stricter
AS_IF([test "x$enable_fatal_warnings" = xyes],
  [TP_ADD_LINKER_FLAG([ERROR_LDFLAGS], [-Wl,--no-copy-dt-needed-entries])])

AC_HEADER_STDC([])
AC_C_INLINE

PKG_CHECK_MODULES([GLIB],
  [glib-2.0 >= 2.26.0, gobject-2.0, json-glib-1.0])

PKG_CHECK_MODULES([DBUS], [dbus-1, dbus-glib-1])

PKG_CHECK_MODULES([REST], [rest-0.7, libxml-2.0])

AC_DEFINE([TP_SEAL_ENABLE], [], [Prevent to use sealed variables])
AC_DEFINE([TP_DISABLE_SINGLE_INCLUDE], [], [Disable single header include])
PKG_CHECK_MODULES([TELEPATHY], [telepathy-glib])

AS_AC_EXPAND(DATADIR, $datadir)
DBUS_SERVICES_DIR="$DATADIR/dbus-1/services"
AC_SUBST(DBUS_SERVICES_DIR)
AC_DEFINE_UNQUOTED(DBUS_SERVICES_DIR, "$DBUS_SERVICES_DIR", [DBus services directory])

AC_DEFINE_UNQUOTED(APP_ID, "3821026", [Application ID])
AC_DEFINE_UNQUOTED(APP_SECRET, "ct9vuyd9UNYXddZF6o46", [Application secret key])
AC_DEFINE_UNQUOTED(API_VER, "5.25", [API version])

AC_CHECK_FUNCS(strnlen)

AC_OUTPUT(Makefile \
          m4/Makefile \
          data/Makefile \
          src/Makefile \
)
