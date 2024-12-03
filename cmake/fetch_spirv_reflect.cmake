include(FetchContent)

function(fetch_spirv_reflect)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        SPIRV-Reflect SYSTEM
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Reflect/
        GIT_TAG 8542f37bd9bb202e6c49dc6a9da364c58c34d2a4 # 2024-08-24
        # GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS 1.3.296
    )

    set(SPIRV_REFLECT_EXECUTABLE     "OFF" CACHE BOOL "" FORCE)
    set(SPIRV_REFLECT_STATIC_LIB     "ON"  CACHE BOOL "" FORCE)
    set(SPIRV_REFLECT_BUILD_TESTS    "OFF" CACHE BOOL "" FORCE)
    set(SPIRV_REFLECT_ENABLE_ASSERTS "ON"  CACHE BOOL "" FORCE)
    set(SPIRV_REFLECT_ENABLE_ASAN    "OFF" CACHE BOOL "" FORCE)

    message(STATUS "${CMAKE_PROJECT_NAME}: Fetching SPIRV-Reflect...")
    FetchContent_MakeAvailable(SPIRV-Reflect)
endfunction()
