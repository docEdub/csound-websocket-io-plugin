include_guard()

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel.")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS None Debug Release RelWithDebInfo MinSizeRel)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

# Turn off RPATH.
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Force make to print the command lines.
set(CMAKE_VERBOSE_MAKEFILE OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(MSVC)
    add_compile_options(/W3)
else()
    add_compile_options(-Wall -Wextra)
endif()

include(CheckCCompilerFlag)

function(add_flag flagname flag)
    check_c_compiler_flag(${flag} ${flagname})
    if(${flagname})
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}" PARENT_SCOPE)
    endif()
endfunction()

if(HAS_VISIBILITY_HIDDEN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
endif()

# Set optimization flags.
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
    add_definitions(-fvisibility=hidden)

    add_flag(HAS_SSE -msse)
    add_flag(HAS_SSE2 -msse2)
    add_flag(HAS_FPMATH_SSE -mfgpath=sse)

    set(CMAKE_C_FLAGS_RELEASE "-O3")
endif()

if(MINGW)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mstackrealign")
endif()

add_flag(HAS_TREE_VECTORIZE -ftree-vectorize)

if(NOT SKIP_FAST_MATH)
  # NB: fast-math causes undefined symbols in linux when built inside an azure pipeline.
  add_flag(HAS_FAST_MATH -ffast-math)
endif()

add_flag(HAS_OMIT_FRAME_POINTER -fomit-frame-pointer)

mark_as_advanced(FORCE
    CMAKE_INSTALL_PREFIX
    CMAKE_OSX_ARCHITECTURES
    CMAKE_OSX_DEPLOYMENT_TARGET
    CMAKE_OSX_SYSROOT
    CSOUND_FRAMEWORK
    CSOUND_FRAMEWORK32
    FETCHCONTENT_BASE_DIR
    FETCHCONTENT_FULLY_DISCONNECTED
    FETCHCONTENT_QUIET
    FETCHCONTENT_SOURCE_DIR_VCPKG
    FETCHCONTENT_UPDATES_DISCONNECTED
    FETCHCONTENT_UPDATES_DISCONNECTED_VCPKG
    VCPKG_APPLOCAL_DEPS
    VCPKG_BOOTSTRAP_OPTIONS
    VCPKG_INSTALLED_DIR
    VCPKG_INSTALL_OPTIONS
    VCPKG_MANIFEST_DIR
    VCPKG_MANIFEST_INSTALL
    VCPKG_MANIFEST_MODE
    VCPKG_OVERLAY_PORTS
    VCPKG_OVERLAY_TRIPLETS
    VCPKG_PREFER_SYSTEM_LIBS
    VCPKG_SETUP_CMAKE_PROGRAM_PATH
    VCPKG_TRACE_FIND_PACKAGE
    VCPKG_TARGET_TRIPLET
    X_VCPKG_APPLOCAL_DEPS_INSTALL
    X_VCPKG_APPLOCAL_DEPS_SERIALIZED
    Z_VCPKG_CL
    _VCPKG_INSTALLED_DIR
    libwebsockets_DIR
)
