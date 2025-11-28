include(FetchContent)

function(fetch_spdlog)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        spdlog SYSTEM
        GIT_REPOSITORY https://github.com/gabime/spdlog
        GIT_TAG 486b55554f11c9cccc913e11a87085b2a91f706f # 2025-10-11
        # GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS 1.16.0
    )

    # Use a different version of fmt from the one that ships with spdlog, but
    # not the standard library implementation
    set(SPDLOG_FMT_EXTERNAL           "ON"  CACHE BOOL "" FORCE)
    set(SPDLOG_USE_STD_FORMAT         "OFF" CACHE BOOL "" FORCE)

    message(STATUS "${CMAKE_PROJECT_NAME}: Fetching spdlog...")
    FetchContent_MakeAvailable(spdlog)
endfunction()
