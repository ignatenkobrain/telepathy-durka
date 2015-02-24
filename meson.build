project('telepathy-durka', 'c')

conf_data = configuration_data()
conf_data.set('VERSION', '"0.0.1"')
if get_option('type') != 'release'
  conf_data.set('ENABLE_DEBUG', true)
else
  conf_data.set('ENABLE_DEBUG', false)
endif
conf_data.set('APP_ID', '"3821026"')
conf_data.set('APP_SECRET', '"ct9vuyd9UNYXddZF6o46"')
conf_data.set('API_VER', '"5.25"')

prefix = get_option('prefix')

dbusservicesdir = get_option('dbusservicesdir')
libexecdir = get_option('libexecdir')
telepathymanagersdir = get_option('telepathymanagersdir')

subdir('src')
subdir('data')
