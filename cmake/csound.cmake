include_guard()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

find_package(CSOUND)

set(CSOUND_API_VERSION "6.0" CACHE STRING "Csound API version; set to 6.0 or 7.0 (default: 6.0)")
set_property(CACHE CSOUND_API_VERSION PROPERTY STRINGS 6.0 7.0)

set(CSOUND_FLOAT_SIZE "DOUBLE" CACHE STRING "Csound float size; set to SINGLE for 32 bit floats or DOUBLE for 64 bit floats (default: DOUBLE)")
set_property(CACHE CSOUND_FLOAT_SIZE PROPERTY STRINGS SINGLE DOUBLE)

set(BUILDING_CSOUND_PLUGINS ON)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(LINUX YES)
else()
    set(LINUX NO)
endif()

# Set Csound plugins directory.
if("${CSOUND_FLOAT_SIZE}" STREQUAL "DOUBLE")
    add_definitions(-DUSE_DOUBLE)
    add_definitions(-DB64BIT)
    set(plugins_subdir "${CSOUND_API_VERSION}/plugins64")
else()
    set(plugins_subdir "${CSOUND_API_VERSION}/plugins")
endif()
if(APPLE)
    set(CSOUND_PLUGINS_DIR "$ENV{HOME}/Library/csound/${plugins_subdir}")
elseif(LINUX)
    set(CSOUND_PLUGINS_DIR "$ENV{HOME}/.local/lib/csound/${plugins_subdir}")
elseif(WIN32)
    set(CSOUND_PLUGINS_DIR "$ENV{LOCALAPPDATA}/csound/${plugins_subdir}")
endif()

# Adds the given `TARGET` with the given `SOURCES` and `LIBRARIES` to the build as a shared library.
function(add_shared_library)
    cmake_parse_arguments(ARG "" "TARGET" "SOURCES;LIBRARIES" ${ARGN})

    if(APPLE)
        add_library(${ARG_TARGET} SHARED ${ARG_SOURCES})
    else()
        add_library(${ARG_TARGET} MODULE ${ARG_SOURCES})
    endif()

    target_link_libraries(${ARG_TARGET} PRIVATE ${ARG_LIBRARIES})
endfunction()

# Adds the given `TARGET` with the given `SOURCES` and `LIBRARIES` to the build as a Csound plugin.
function(add_csound_plugin)
    cmake_parse_arguments(ARG "" "TARGET" "SOURCES;LIBRARIES" ${ARGN})

    add_shared_library(TARGET ${ARG_TARGET} SOURCES ${ARG_SOURCES} LIBRARIES ${ARG_LIBRARIES})

    target_include_directories(${ARG_TARGET} PRIVATE "${CSOUND_INCLUDE_DIR}")

    set_property(TARGET ${ARG_TARGET} PROPERTY C_STANDARD 99)
    set_target_properties(${ARG_TARGET} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    install(TARGETS ${ARG_TARGET} LIBRARY DESTINATION "${CSOUND_PLUGINS_DIR}")
endfunction()
