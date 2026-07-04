cmake_minimum_required(VERSION 3.20)
project(OpenSTKPConnector LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT APPLE)
    message(FATAL_ERROR "Questo skeleton è configurato per macOS/X-Plane.")
endif()

if(NOT DEFINED ENV{XPLANE_SDK})
    message(FATAL_ERROR "Imposta XPLANE_SDK, es: export XPLANE_SDK=$HOME/Development/XPSDK")
endif()

set(XPLANE_SDK "$ENV{XPLANE_SDK}")
set(XPLANE_HEADERS "${XPLANE_SDK}/CHeaders")

add_library(OpenSTKPConnector MODULE
    src/plugin.cpp
    src/protocol.cpp
    src/tcp_server.cpp
    src/datarefs.cpp
    src/logger.cpp
)

target_include_directories(OpenSTKPConnector PRIVATE
    include
    "${XPLANE_HEADERS}/XPLM"
    "${XPLANE_HEADERS}/Widgets"
)

target_compile_definitions(OpenSTKPConnector PRIVATE
    XPLM200=1
    XPLM210=1
    XPLM300=1
    XPLM301=1
    XPLM400=1
    APL=1
    IBM=0
    LIN=0
)

set_target_properties(OpenSTKPConnector PROPERTIES
    PREFIX ""
    SUFFIX ".xpl"
    OUTPUT_NAME "mac"
    BUNDLE OFF
)

target_link_options(OpenSTKPConnector PRIVATE
    "-undefined" "dynamic_lookup"
)

add_custom_command(TARGET OpenSTKPConnector POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${CMAKE_SOURCE_DIR}/dist/stkpconnector"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:OpenSTKPConnector>"
        "${CMAKE_SOURCE_DIR}/dist/stkpconnector/mac.xpl"
)
