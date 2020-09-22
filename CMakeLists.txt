# Initialize packages
cmake_minimum_required(VERSION 3.5)
project(pvr.sql)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(p8-platform REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(${p8-platform_INCLUDE_DIRS}
                    ${KODI_INCLUDE_DIR}/.. # Hack way with "/..", need bigger Kodi cmake rework to match right include ways
                    ${ZLIB_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/lib)

addon_version(pvr.sql IPTV)
add_definitions(-DIPTV_VERSION=${IPTV_VERSION})

# Add sockets deps
set(MISC_LIBS ${MISC_LIBS})

if (MSVC)
  set(MISC_LIBS ${MISC_LIBS} ws2_32.lib)
endif(MSVC)

# Sources and headers
set(SQL_SOURCES 
                src/client.cpp
                src/IPTVClient.cpp
                src/DVRClient.cpp)


set(SQL_SOURCES_IPTV
                src/pvrsimple/TCPServer.cpp
                src/pvrsimple/TCPClient.cpp
                src/pvrsimple/SQLConnection.cpp
                src/pvrsimple/PVRRecorder.cpp
                src/pvrsimple/Settings.cpp)


set(SQL_SOURCES_IPTV_UTILITIES
                src/pvrsimple/data/SQLRecord.cpp
                src/pvrsimple/data/IPTVChannel.cpp
                src/pvrsimple/data/IPTVChannelGroup.cpp
                src/pvrsimple/data/IPTVChannelGroupMember.cpp
                src/pvrsimple/data/IPTVEpgChannel.cpp
                src/pvrsimple/data/IPTVEpgEntry.cpp
                src/pvrsimple/data/DVRTimerType.cpp
                src/pvrsimple/data/DVRTimer.cpp
                src/pvrsimple/data/DVRRecording.cpp
                src/pvrsimple/utilities/Callbacks.cpp
                src/pvrsimple/utilities/FileHelpers.cpp
                src/pvrsimple/utilities/HTTPHelpers.cpp
                src/pvrsimple/utilities/LOGHelpers.cpp
                src/pvrsimple/utilities/M3UHelpers.cpp
                src/pvrsimple/utilities/GZHelpers.cpp
                src/pvrsimple/utilities/XMLHelpers.cpp
                src/pvrsimple/utilities/SQLHelpers.cpp
                src/pvrsimple/utilities/Subprocess.cpp
                src/pvrsimple/utilities/Utilities.cpp)

source_group("Source Files" FILES ${SQL_SOURCES})
source_group("Source Files\\iptv" FILES ${SQL_SOURCES_IPTV})
source_group("Source Files\\iptv\\utilities" FILES ${SQL_SOURCES_IPTV_UTILITIES})

# Resource files
set(SQL_RESOURCES 
                  README.md
                  pvr.sql/addon.xml
                  pvr.sql/changelog.txt
                  pvr.sql/icon.png
                  pvr.sql/resources/Settings.xml
                  pvr.sql/resources/language/resource.language.en_gb/strings.po)

source_group("Resource Files" FILES ${SQL_RESOURCES})

# Combine the file lists & build addon
list(APPEND   SQL_SOURCES
            ${SQL_SOURCES_IPTV}
            ${SQL_SOURCES_IPTV_UTILITIES})

add_subdirectory(lib/sqlite)
add_subdirectory(lib/netsockets)

set(DEPLIBS ${p8-platform_LIBRARIES} ${ZLIB_LIBRARIES} sqlite netsockets ${MISC_LIBS})

build_addon(pvr.sql SQL DEPLIBS)

include(CPack)
