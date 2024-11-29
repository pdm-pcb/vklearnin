include(FetchContent)

function(fetch_spirv_cross)
    # set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        SPIRV-Cross SYSTEM
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross/
        GIT_TAG 0e2880ab990e79ce6cc8c79c219feda42d98b1e8 # 2024-04-27
        # GIT_PROGRESS TRUE
    )

    set(SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS "ON"  CACHE BOOL "Instead of throwing exceptions assert" FORCE)
    set(SPIRV_CROSS_SHARED                   "OFF" CACHE BOOL "Build the C API as a single shared library." FORCE)
    set(SPIRV_CROSS_STATIC                   "ON"  CACHE BOOL "Build the C and C++ API as static libraries." FORCE)
    set(SPIRV_CROSS_CLI                      "OFF" CACHE BOOL "Build the CLI binary. Requires SPIRV_CROSS_STATIC." FORCE)
    set(SPIRV_CROSS_ENABLE_TESTS             "OFF" CACHE BOOL "Enable SPIRV-Cross tests." FORCE)
    set(SPIRV_CROSS_ENABLE_GLSL              "ON"  CACHE BOOL "Enable GLSL support." FORCE)
    set(SPIRV_CROSS_ENABLE_HLSL              "OFF" CACHE BOOL "Enable HLSL target support." FORCE)
    set(SPIRV_CROSS_ENABLE_MSL               "OFF" CACHE BOOL "Enable MSL target support." FORCE)
    set(SPIRV_CROSS_ENABLE_CPP               "OFF" CACHE BOOL "Enable C++ target support." FORCE)
    set(SPIRV_CROSS_ENABLE_REFLECT           "ON"  CACHE BOOL "Enable JSON reflection target support." FORCE)
    set(SPIRV_CROSS_ENABLE_C_API             "OFF" CACHE BOOL "Enable C API wrapper support in static library." FORCE)
    set(SPIRV_CROSS_ENABLE_UTIL              "ON"  CACHE BOOL "Enable util module support." FORCE)
    set(SPIRV_CROSS_SANITIZE_ADDRESS         "OFF" CACHE BOOL "Sanitize address" FORCE)
    set(SPIRV_CROSS_SANITIZE_MEMORY          "OFF" CACHE BOOL "Sanitize memory" FORCE)
    set(SPIRV_CROSS_SANITIZE_THREADS         "OFF" CACHE BOOL "Sanitize threads" FORCE)
    set(SPIRV_CROSS_SANITIZE_UNDEFINED       "OFF" CACHE BOOL "Sanitize undefined" FORCE)
    set(SPIRV_CROSS_FORCE_STL_TYPES          "OFF" CACHE BOOL "Force use of STL types instead of STL replacements in certain places. Might reduce performance." FORCE)
    set(SPIRV_CROSS_SKIP_INSTALL             "ON"  CACHE BOOL "Skips installation targets." FORCE)
    set(SPIRV_CROSS_WERROR                   "OFF" CACHE BOOL "Fail build on warnings." FORCE)
    set(SPIRV_CROSS_MISC_WARNINGS            "OFF" CACHE BOOL "Misc warnings useful for Travis runs." FORCE)
    set(SPIRV_CROSS_FORCE_PIC                "OFF" CACHE BOOL "Force position-independent code for all targets." FORCE)

    message(STATUS "${CMAKE_PROJECT_NAME}: Fetching SPIRV-Cross...")
    FetchContent_MakeAvailable(SPIRV-Cross)
endfunction()
