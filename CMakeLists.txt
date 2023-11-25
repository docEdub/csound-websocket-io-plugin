cmake_minimum_required(VERSION 3.14)

include("cmake/vcpkg.cmake")

project(csound-websocket-io-plugin)

include("cmake/config.cmake")
include("cmake/csound.cmake")

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
