include_guard()

# NB: Include this file before calling `project(...)` in CMakeLists.txt, so the vcpkg toolchain gets used.

message(STATUS "Fetching vcpkg")
include(FetchContent)
FetchContent_Declare(
    vcpkg
    GIT_REPOSITORY https://github.com/microsoft/vcpkg.git
    GIT_TAG 2023.11.20
)
FetchContent_MakeAvailable(vcpkg)
message(STATUS "Fetching vcpkg - done")

set(VCPKG_ROOT "${vcpkg_SOURCE_DIR}")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
set(X_VCPKG_APPLOCAL_DEPS_INSTALL ON CACHE BOOL "" FORCE)
