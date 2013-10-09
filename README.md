telepathy-durka
=============

VK Connection Manager for Telepathy.

Build requirements
------------------

* cmake
* make
* glib2-devel
* telepathy-glib-devel
* dbus-devel
* libxml2-devel
* json-parser-devel

Requirements
------------

* libxml2
* telepathy-glib
* dbus
* json-parser

How to install
--------------

```
$ mkdir build
$ cd build
$ cmake ../ -DCMAKE_INSTALL_PREFIX=/usr
$ make
# make install
```

How to use
----------

1. /usr/libexec/telepathy-durka
2. Open empathy-accounts or gnome-online-accounts
3. Add vk account

Token authorize
---------------

At adding account use wrong password.

Open link: https://oauth.vk.com/authorize?client_id=3821026&scope=friends,messages,offline&redirect_uri=https://oauth.vk.com/blank.html&v=5.0&response_type=token

In response you will have redirect and in address-string you will have alike:

```
#access_token=[THIS]&expires_in=0&user_id=1361145746
```

1. Open additional settings
2. Into Token-parameter enter "THIS"

Connection manager prefer token if it set.
