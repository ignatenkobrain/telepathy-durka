project(telepahty-durka C)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_INSTALL_PREFIX "/usr")

set(DURKA_MAJOR_VERSION 0)
set(DURKA_MINOR_VERSION 0)
set(DURKA_MICRO_VERSION 0)
set(DURKA_NANO_VERSION 1)

set(DURKA_BASE_VERSION ${DURKA_MAJOR_VERSION}.${DURKA_MINOR_VERSION}.${DURKA_MICRO_VERSION})
if(DURKA_NANO_VERSION EQUAL 0)
  set(DURKA_VERSION ${DURKA_BASE_VERSION})
else(DURKA_NANO_VERSION EQUAL 0)
  set(DURKA_VERSION ${DURKA_BASE_VERSION}.${DURKA_NANO_VERSION})
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
endif(DURKA_NANO_VERSION EQUAL 0)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra -Wformat -Wformat-security -pipe")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter -Wno-missing-field-initializers")

message(STATUS "Version: ${DURKA_VERSION}")

include(FindPkgConfig)

SET(SRCS
        src/contact-list.c
        src/conn.c
        src/protocol.c
        src/connection-manager.c
        src/main.c
   )

set(HDRS
        src/contact-list.h
        src/conn.h
        src/protocol.h
        src/connection-manager.h
   )

pkg_search_module(GLIB REQUIRED glib-2.0>=2.26)
pkg_search_module(TELEPATHY_GLIB REQUIRED telepathy-glib>=0.13.9)
pkg_search_module(GOBJECT REQUIRED gobject-2.0)
pkg_search_module(REST REQUIRED rest-0.7)
find_package(LibXml2)

if (NOT LIBXML2_FOUND)
    message(FATAL_ERROR "libxml2 not found")
endif(NOT LIBXML2_FOUND)

set(DURKA_TELEPATHY_MANAGER_DIR "${CMAKE_INSTALL_PREFIX}/share/telepathy/managers" CACHE PATH "Telepathy manager-files directory")
set(DURKA_TELEPATHY_EXEC "${CMAKE_INSTALL_PREFIX}/libexec/telepathy-durka" CACHE FILEPATH "Telepathy executable")

set(ENABLE_DEBUG true CACHE BOOLEAN "Enable debug messages and assertions")

configure_file("${CMAKE_SOURCE_DIR}/data/org.freedesktop.Telepathy.ConnectionManager.durka.service.in"
               "${CMAKE_BINARY_DIR}/org.freedesktop.Telepathy.ConnectionManager.durka.service")
configure_file("${CMAKE_SOURCE_DIR}/src/config.h.in"
               "${CMAKE_BINARY_DIR}/config.h")

pkg_search_module(DBUS dbus-1)
pkg_search_module(DBUS-GLIB dbus-glib-1)

if (DBUS_FOUND)
    exec_program(${PKG_CONFIG_EXECUTABLE} ARGS dbus-1 --variable session_bus_services_dir OUTPUT_VARIABLE __return)
    set(DURKA_DBUS_SERVICE_DIR ${__return} CACHE PATH "DBus service directory")
else (DBUS_FOUND)
    set(DURKA_DBUS_SERVICE_DIR "${CMAKE_INSTALL_PREFIX}/share/dbus-1/services" CACHE PATH "DBus service directory")
endif (DBUS_FOUND)

set(JSON_INCLUDE_DIR "/usr/include/json-parser")
set(JSON_LIBRARY_DIR "/usr/lib64")
message(STATUS "json-parser: ${JSON_INCLUDE_DIR} ${JSON_LIBRARY_DIR}/libjsonparser.so")

include_directories(${GLIB_INCLUDE_DIRS})
include_directories(${DBUS-GLIB_INCLUDE_DIRS})
include_directories(${TELEPATHY_GLIB_INCLUDE_DIRS})
include_directories(${GOBJECT_INCLUDE_DIRS})
include_directories(${REST_INCLUDE_DIRS})
include_directories(${JSON_INCLUDE_DIR})
include_directories(${DURKA_INCLUDE_DIRS})
include_directories(${LIBXML2_INCLUDE_DIR})
include_directories(${CMAKE_BINARY_DIR})

link_directories(${DURKA_LIBRARY_DIRS})
link_directories(${LIBXML2_LIBRARY_DIRS})

add_executable(telepathy-durka
    ${SRCS}
    ${HDRS}
)

target_link_libraries(telepathy-durka ${GLIB_LIBRAIRES})
target_link_libraries(telepathy-durka ${DBUS-GLIB_LIBRARIES})
target_link_libraries(telepathy-durka ${TELEPATHY_GLIB_LIBRARIES})
target_link_libraries(telepathy-durka ${GOBJECT_LIBRARIES})
target_link_libraries(telepathy-durka ${REST_LIBRARIES})
target_link_libraries(telepathy-durka ${JSON_LIBRARY_DIR}/libjsonparser.so)
target_link_libraries(telepathy-durka ${DURKA_LIBRARIES})
target_link_libraries(telepathy-durka ${LIBXML2_LIBRARIES})

install(TARGETS telepathy-durka
        RUNTIME DESTINATION libexec)

install(FILES "${CMAKE_BINARY_DIR}/org.freedesktop.Telepathy.ConnectionManager.durka.service"
        DESTINATION ${DURKA_DBUS_SERVICE_DIR})

install(FILES "${CMAKE_SOURCE_DIR}/data/durka.manager"
        DESTINATION ${DURKA_TELEPATHY_MANAGER_DIR})


