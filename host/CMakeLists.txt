include(GNUInstallDirs)

set(HOST_SOURCES main.cpp
                connection.cpp
                mprisplugin.cpp
                abstractbrowserplugin.cpp
                )

qt5_add_dbus_adaptor(HOST_SOURCES ../dbus/org.mpris.MediaPlayer2.xml mprisplugin.h MPrisPlugin mprisroot MPrisRoot)
qt5_add_dbus_adaptor(HOST_SOURCES ../dbus/org.mpris.MediaPlayer2.Player.xml mprisplugin.h MPrisPlugin mprisplayer MPrisPlayer)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
add_executable(mpris-browser-integration-host ${HOST_SOURCES})
target_link_libraries(
    mpris-browser-integration-host
    Qt5::DBus
)

install(TARGETS mpris-browser-integration-host
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
