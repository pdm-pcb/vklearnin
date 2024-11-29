include(FetchContent)

function(fetch_spdlog)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        spdlog SYSTEM
        GIT_REPOSITORY https://github.com/gabime/spdlog
        GIT_TAG 27cb4c76708608465c413f6d0e6b8d99a4d84302
        # GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS 1.14
    )

    # Use a different version of fmt from the one that ships with spdlog, but
    # not the standard library implementation
    set(SPDLOG_FMT_EXTERNAL           "ON"  CACHE BOOL "" FORCE)
    set(SPDLOG_USE_STD_FORMAT         "OFF" CACHE BOOL "" FORCE)

    # All other options off
    set(SPDLOG_BUILD_ALL              "OFF" CACHE BOOL "" FORCE)
    set(SPDLOG_BUILD_SHARED           "OFF" CACHE BOOL "" FORCE)
    set(SPDLOG_ENABLE_PCH             "OFF" CACHE BOOL "" FORCE)
    set(SPDLOG_BUILD_PIC              "OFF" CACHE BOOL "" FORCE)
    set(SPDLOG_BUILD_EXAMPLE          "OFF" CACHE BOOL "" FORCE)
    set(SPDLOG_BUILD_EXAMPLE_HO       "OFF" CACHE BOOL "" FORCE)
    set(SPDLOG_BUILD_TESTS            "OFF" CACHE BOOL "" FORCE)
    set(SPDLOG_BUILD_TESTS_HO         "OFF" CACHE BOOL "" FORCE)
    set(SPDLOG_BUILD_BENCH            "OFF" CACHE BOOL "" FORCE)
    set(SPDLOG_SANITIZE_ADDRESS       "OFF" CACHE BOOL "" FORCE)
    set(SPDLOG_BUILD_WARNINGS         "OFF" CACHE BOOL "" FORCE)
    set(SPDLOG_SYSTEM_INCLUDES        "OFF" CACHE BOOL "" FORCE)
    set(SPDLOG_INSTALL                "OFF" CACHE BOOL "" FORCE)
    set(SPDLOG_FMT_EXTERNAL_HO        "OFF" CACHE BOOL "" FORCE)
    set(SPDLOG_NO_EXCEPTIONS          "OFF" CACHE BOOL "" FORCE)
    set(SPDLOG_WCHAR_SUPPORT          "OFF" CACHE BOOL "" FORCE)
    set(SPDLOG_WCHAR_FILENAMES        "OFF" CACHE BOOL "" FORCE)
    set(SPDLOG_CLOCK_COARSE           "OFF" CACHE BOOL "" FORCE)
    set(SPDLOG_PREVENT_CHILD_FD       "OFF" CACHE BOOL "" FORCE)
    set(SPDLOG_NO_THREAD_ID           "OFF" CACHE BOOL "" FORCE)
    set(SPDLOG_NO_TLS                 "OFF" CACHE BOOL "" FORCE)
    set(SPDLOG_NO_ATOMIC_LEVELS       "OFF" CACHE BOOL "" FORCE)
    set(SPDLOG_DISABLE_DEFAULT_LOGGER "OFF" CACHE BOOL "" FORCE)
    set(SPDLOG_TIDY                   "OFF" CACHE BOOL "" FORCE)

    message(STATUS "${CMAKE_PROJECT_NAME}: Fetching spdlog...")
    FetchContent_MakeAvailable(spdlog)
endfunction()
