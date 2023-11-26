# CMake 3.24 is required for CMAKE_PROJECT_TOP_LEVEL_INCLUDES used in CMakeCsoundPlugin.
# See https://cmake.org/cmake/help/v3.24/variable/CMAKE_PROJECT_TOP_LEVEL_INCLUDES.html.
cmake_minimum_required(VERSION 3.24)

add_subdirectory(CMakeCsoundPlugin)

project(csound-websocket-io-plugin)

find_package(libwebsockets REQUIRED)

add_csound_plugin(
    TARGET websocket-io
    SOURCES
        "src/_init_.c"
        "src/common.c"
        "src/common.h"
        "src/get.c"
        "src/get.h"
        "src/set.c"
        "src/set.h"
    LIBRARIES
        websockets
)
