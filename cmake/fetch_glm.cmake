include(FetchContent)

function(fetch_glm)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        glm SYSTEM
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG 0af55ccecd98d4e5a8d1fad7de25ba429d60e863
        # GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS 1.0
    )

    # All options off
    set(GLM_ENABLE_CXX_20 "ON" CACHE BOOL "" FORCE)

    message(STATUS "${CMAKE_PROJECT_NAME}: Fetching glm...")
    FetchContent_MakeAvailable(glm)
endfunction()
