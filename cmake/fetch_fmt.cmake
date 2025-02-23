include(FetchContent)

function(fetch_fmt)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        fmt SYSTEM
        GIT_REPOSITORY https://github.com/fmtlib/fmt/
        GIT_TAG 9cf9f38eded63e5e0fb95cd536ba51be601d7fa2 # 2025-01-25
        # GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS 11.1.3
    )

    # Disable the install target as we'll be linking statically
    set(FMT_INSTALL "OFF" CACHE BOOL "" FORCE)

    message(STATUS "${CMAKE_PROJECT_NAME}: Fetching fmt...")
    FetchContent_MakeAvailable(fmt)
endfunction()
