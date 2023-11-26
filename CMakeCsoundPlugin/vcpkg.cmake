include_guard()

# NB: Include this file before calling `project(...)` in CMakeLists.txt, so the vcpkg toolchain gets used.

message(STATUS "Fetching vcpkg")
include(FetchContent)
FetchContent_Declare(
    vcpkg
    URL https://github.com/microsoft/vcpkg/archive/refs/tags/2023.11.20.tar.gz
    URL_MD5 24d44e89fc845d67051a52ed5066fc84
    DOWNLOAD_DIR "${FETCHCONTENT_BASE_DIR}/vcpkg"
    DOWNLOAD_EXTRACT_TIMESTAMP true
)
FetchContent_MakeAvailable(vcpkg)
message(STATUS "Fetching vcpkg - done")

set(VCPKG_ROOT "${vcpkg_SOURCE_DIR}")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
set(X_VCPKG_APPLOCAL_DEPS_INSTALL ON CACHE BOOL "" FORCE)
