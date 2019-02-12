cmake_minimum_required(VERSION 3.0)

project(mpris-browser-integration)
set(PROJECT_VERSION "5.15.80")
set(PROJECT_VERSION_MAJOR 5)

set(QT_MIN_VERSION "5.9.0")

include(FeatureSummary)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
Gui
DBus
Widgets
)

add_definitions(-DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT)
#add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)

# Options

# in ubuntu and derivatives, this should be installed in the chromium-browser/extensions,
# but since this is distribution specific path, let's use what upstream uses
# ideally ubuntu and derivatives should patch this in their packaging
set(CHROMIUM_EXTENSIONS_DIR "chromium" CACHE STRING "Directory name to install Chromium extensions into")
add_feature_info(CHROMIUM_EXTENSIONS_DIR On "Directory name to install Chromium extensions into is '${CHROMIUM_EXTENSIONS_DIR}'")

option(COPY_MESSAGING_HOST_FILE_HOME "Copy the native messaging hosts json file to user home dir" FALSE)
add_feature_info(COPY_MESSAGING_HOST_FILE_HOME ${COPY_MESSAGING_HOST_FILE_HOME} "Enable this option to copy the native messaging hosts json file to home dir if you install plasma-browser-integration to custom prefix (non-/usr)")

set(MOZILLA_DIR "${CMAKE_INSTALL_PREFIX}/lib/mozilla" CACHE STRING "Mozilla directory")
add_feature_info(MOZILLA_DIR On "Mozilla directory is '${MOZILLA_DIR}'")

add_subdirectory(host)

if(NOT DEFINED CHROME_EXTENSION_ID)
    # The extension ID is based on the key used to sign the extension
    # see https://stackoverflow.com/questions/23873623/obtaining-chrome-extension-id-for-development
    set(CHROME_EXTENSION_ID "cimiefiiaegbelhefglklhhakcgmhkai")
endif()

# TODO configure manifest.json

configure_file(com.github.rgeorgiev583.mpris.chrome_integration.json.in com.github.rgeorgiev583.mpris.chrome_integration.json @ONLY)
configure_file(com.github.rgeorgiev583.mpris.firefox_integration.json.in com.github.rgeorgiev583.mpris.firefox_integration.json @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/com.github.rgeorgiev583.mpris.firefox_integration.json DESTINATION ${MOZILLA_DIR}/native-messaging-hosts/ RENAME com.github.rgeorgiev583.mpris.browser_integration.json)

if (COPY_MESSAGING_HOST_FILE_HOME)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/com.github.rgeorgiev583.mpris.firefox_integration.json DESTINATION $ENV{HOME}/.mozilla/native-messaging-hosts/ RENAME com.github.rgeorgiev583.mpris.browser_integration.json)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/com.github.rgeorgiev583.mpris.chrome_integration.json DESTINATION $ENV{HOME}/.config/chromium/NativeMessagingHosts/ RENAME com.github.rgeorgiev583.mpris.browser_integration.json)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/com.github.rgeorgiev583.mpris.chrome_integration.json DESTINATION $ENV{HOME}/.config/google-chrome/NativeMessagingHosts/ RENAME com.github.rgeorgiev583.mpris.browser_integration.json)
endif()

# TODO firefox

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
