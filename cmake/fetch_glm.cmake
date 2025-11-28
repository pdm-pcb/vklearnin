include(FetchContent)

function(fetch_glm)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        glm SYSTEM
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG a532f5b1cf27d6a3c099437e6959cf7e398a0a67 # 2025-10-15
        # GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS 1.0.2
    )

    # All options off
    set(GLM_ENABLE_CXX_20 "ON" CACHE BOOL "" FORCE)

    message(STATUS "${CMAKE_PROJECT_NAME}: Fetching glm...")
    FetchContent_MakeAvailable(glm)
endfunction()
