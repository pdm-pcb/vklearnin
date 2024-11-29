include(FetchContent)

function(fetch_spirv_reflect)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        SPIRV-Reflect SYSTEM
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Reflect/
        GIT_TAG b4dc70d8e6ac30c719a2d05b8ad05e1d277c92b4 # 2024-04-27
        # GIT_PROGRESS TRUE
    )

    set(SPIRV_REFLECT_EXECUTABLE     "OFF" CACHE BOOL "" FORCE)
    set(SPIRV_REFLECT_STATIC_LIB     "ON"  CACHE BOOL "" FORCE)
    set(SPIRV_REFLECT_BUILD_TESTS    "OFF" CACHE BOOL "" FORCE)
    set(SPIRV_REFLECT_ENABLE_ASSERTS "ON"  CACHE BOOL "" FORCE)
    set(SPIRV_REFLECT_ENABLE_ASAN    "OFF" CACHE BOOL "" FORCE)

    message(STATUS "${CMAKE_PROJECT_NAME}: Fetching SPIRV-Reflect...")
    FetchContent_MakeAvailable(SPIRV-Reflect)
endfunction()
