CLEANFILES = $(BUILT_SOURCES)

libexec_PROGRAMS=telepathy-durka

libdurka_la_SOURCES = \
	conn.c \
	conn.h \
	protocol.c \
	protocol.h \
	connection-manager.c \
	connection-manager.h \
	contact-list.c \
	contact-list.h \
	conn-contact-info.c \
	conn-contact-info.h \
	$(NULL)

nodist_libdurka_la_SOURCES = \
	$(BUILT_SOURCES)

telepathy_durka_SOURCES = \
	durka.c

telepathy_durka_LDADD = \
	libdurka.la \
	$(ALL_LIBS)

noinst_LTLIBRARIES = libdurka.la

AM_CFLAGS = \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	$(ERROR_CFLAGS) \
	@DBUS_CFLAGS@ \
	@GLIB_CFLAGS@ \
	@REST_CFLAGS@ \
	@TELEPATHY_CFLAGS@ \
	-std=c99

AM_LDFLAGS = \
	$(ERROR_LDFLAGS) \
	$(NULL)

ALL_LIBS = \
	@DBUS_LIBS@ \
	@GLIB_LIBS@ \
	@REST_LIBS@ \
	@TELEPATHY_LIBS@

-include $(top_srcdir)/git.mk
