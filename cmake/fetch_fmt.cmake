include(FetchContent)

function(fetch_fmt)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        fmt SYSTEM
        GIT_REPOSITORY https://github.com/fmtlib/fmt/
        GIT_TAG 407c905e45ad75fc29bf0f9bb7c5c2fd3475976f # 2025-10-29
        # GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS 12.1.0
    )

    # Disable the install target as we'll be linking statically
    set(FMT_INSTALL "OFF" CACHE BOOL "" FORCE)

    message(STATUS "${CMAKE_PROJECT_NAME}: Fetching fmt...")
    FetchContent_MakeAvailable(fmt)
endfunction()
