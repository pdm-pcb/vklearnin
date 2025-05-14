include(FetchContent)

function(fetch_cpptrace)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        cpptrace SYSTEM
        GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace
        GIT_TAG ce639ebfcec47a7c74233b4bab50017cb34e615b # 2025-04-08
        # GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS 0.8.3
    )

    message(STATUS "${CMAKE_PROJECT_NAME}: Fetching cpptrace...")
    FetchContent_MakeAvailable(cpptrace)
endfunction()
