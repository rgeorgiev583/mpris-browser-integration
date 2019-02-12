cmake_minimum_required(VERSION 3.0)

project(mpris-browser-integration)

set(QT_MIN_VERSION "5.9.0")

include(FeatureSummary)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS DBus)

add_definitions(-DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT)
#add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)

# Options

option(COPY_MESSAGING_HOST_FILE_HOME "Copy the native messaging hosts json file to user home dir" TRUE)
add_feature_info(COPY_MESSAGING_HOST_FILE_HOME ${COPY_MESSAGING_HOST_FILE_HOME} "Enable this option to copy the native messaging hosts json file to home dir if you install plasma-browser-integration to custom prefix (non-/usr)")

add_subdirectory(host)

if(NOT DEFINED CHROME_EXTENSION_ID)
    # The extension ID is based on the key used to sign the extension
    # see https://stackoverflow.com/questions/23873623/obtaining-chrome-extension-id-for-development
    set(CHROME_EXTENSION_ID "cimiefiiaegbelhefglklhhakcgmhkai")
endif()

# TODO configure manifest.json

configure_file(com.github.rgeorgiev583.mpris.chrome_integration.json.in com.github.rgeorgiev583.mpris.chrome_integration.json @ONLY)
configure_file(com.github.rgeorgiev583.mpris.firefox_integration.json.in com.github.rgeorgiev583.mpris.firefox_integration.json @ONLY)

if (COPY_MESSAGING_HOST_FILE_HOME)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/com.github.rgeorgiev583.mpris.firefox_integration.json DESTINATION $ENV{HOME}/.mozilla/native-messaging-hosts/ RENAME com.github.rgeorgiev583.mpris.browser_integration.json)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/com.github.rgeorgiev583.mpris.chrome_integration.json DESTINATION $ENV{HOME}/.config/chromium/NativeMessagingHosts/ RENAME com.github.rgeorgiev583.mpris.browser_integration.json)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/com.github.rgeorgiev583.mpris.chrome_integration.json DESTINATION $ENV{HOME}/.config/google-chrome/NativeMessagingHosts/ RENAME com.github.rgeorgiev583.mpris.browser_integration.json)
endif()

# TODO firefox

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
