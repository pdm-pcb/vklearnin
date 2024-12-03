include(FetchContent)

function(fetch_fmt)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        fmt SYSTEM
        GIT_REPOSITORY https://github.com/fmtlib/fmt/
        GIT_TAG 0c9fce2ffefecfdce794e1859584e25877b7b592 # 2024-07-20
        # GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS 11.0
    )

    # All options off
    set(FMT_PEDANTIC       "OFF" CACHE BOOL "" FORCE)
    set(FMT_WERROR         "OFF" CACHE BOOL "" FORCE)
    set(FMT_DOC            "OFF" CACHE BOOL "" FORCE)
    set(FMT_INSTALL        "OFF" CACHE BOOL "" FORCE)
    set(FMT_TEST           "OFF" CACHE BOOL "" FORCE)
    set(FMT_FUZZ           "OFF" CACHE BOOL "" FORCE)
    set(FMT_CUDA_TEST      "OFF" CACHE BOOL "" FORCE)
    set(FMT_OS             "OFF" CACHE BOOL "" FORCE)
    set(FMT_MODULE         "OFF" CACHE BOOL "" FORCE)
    set(FMT_SYSTEM_HEADERS "OFF" CACHE BOOL "" FORCE)

    message(STATUS "${CMAKE_PROJECT_NAME}: Fetching fmt...")
    FetchContent_MakeAvailable(fmt)
endfunction()
