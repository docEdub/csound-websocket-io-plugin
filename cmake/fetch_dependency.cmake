include_guard()

include(FetchContent)

macro(fetch_dependency)
    cmake_parse_arguments(ARG "" "NAME;URL;URL_MD5" "" ${ARGN})

    message(STATUS "Fetching ${ARG_NAME}")

    FetchContent_Declare(
        ${ARG_NAME}
        URL ${ARG_URL}
        URL_MD5 ${ARG_URL_MD5}
        DOWNLOAD_DIR "${FETCHCONTENT_BASE_DIR}/${ARG_NAME}"
        DOWNLOAD_EXTRACT_TIMESTAMP true
    )
    FetchContent_MakeAvailable(${ARG_NAME})

    message(STATUS "Fetching ${ARG_NAME} - done")
endmacro()
