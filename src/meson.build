dbus = dependency('dbus-glib-1')
glib = dependency('glib-2.0')
json = dependency('json-glib-1.0')
rest = dependency('rest-0.7')
telepathy = dependency('telepathy-glib')

configure_file(input : 'config.h.in',
               output : 'config.h',
               configuration : conf_data)

libdurka = static_library('durka',
                          'durka-connection.c', 'durka-connection.h',
                          'durka-protocol.c', 'durka-protocol.h',
                          'durka-connection-manager.c', 'durka-connection-manager.h',
                          'durka-contact-list.c', 'durka-contact-list.h',
                          'conn-contact-info.c', 'conn-contact-info.h',
                          include_directories : include_directories('.'),
                          dependencies : [dbus, glib, json, rest, telepathy],
                          install : false)

libexecdir = get_option('libexecdir')
executable('telepathy-durka',
           'durka-main.c',
           include_directories : include_directories('.'),
           dependencies : telepathy,
           link_with : libdurka,
           install : true,
           install_dir : libexecdir)
