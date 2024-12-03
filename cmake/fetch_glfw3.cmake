include(FetchContent)

function(fetch_glfw3)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        glfw3 SYSTEM
        GIT_REPOSITORY https://github.com/glfw/glfw/
        GIT_TAG 7b6aead9fb88b3623e3b3725ebb42670cbe4c579 # 2024-02-23
        # GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS 3.4
    )

    # brasstacks will link the Vulkan library statically, and if we're building
    # for Linux, prefer the X11 backend
    set(GLFW_VULKAN_STATIC  "ON"  CACHE BOOL "" FORCE)
    set(GLFW_BUILD_X11      "ON"  CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WAYLAND  "OFF" CACHE BOOL "" FORCE)

    # All other options off
    set(GLFW_BUILD_EXAMPLES "OFF" CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS    "OFF" CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS     "OFF" CACHE BOOL "" FORCE)
    set(GLFW_INSTALL        "OFF" CACHE BOOL "" FORCE)

    message(STATUS "${CMAKE_PROJECT_NAME}: Fetching glfw3...")
    FetchContent_MakeAvailable(glfw3)
endfunction()
