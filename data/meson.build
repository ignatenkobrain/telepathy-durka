install_data(sources : 'durka.manager',
             install_dir : telepathymanagersdir)

conf_data = configuration_data()
tp_durka_exec = prefix + '/' + libexecdir + '/telepathy-durka'
conf_data.set('TELEPATHY_DURKA_EXEC', tp_durka_exec)
configure_file(input : 'org.freedesktop.Telepathy.ConnectionManager.durka.service.in',
               output : 'org.freedesktop.Telepathy.ConnectionManager.durka.service',
               configuration : conf_data)

install_data(sources : 'org.freedesktop.Telepathy.ConnectionManager.durka.service',
             install_dir : dbusservicesdir)
