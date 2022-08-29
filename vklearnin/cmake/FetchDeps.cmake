include(FetchContent)

# ------------------------------------------------------------------------------
# fmt --------------------------------------------------------------------------
message(NOTICE "Fetch fmt")
FetchContent_Declare(
    fmt 8.1.1 # Arch is a little behind the release scheudle for this lib
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG c4ee726532178e556d923372f29163bd206d7732 # v9.0.0 | 2022-07-04
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(fmt)

# ------------------------------------------------------------------------------
# spdlog -----------------------------------------------------------------------
message(NOTICE "Fetch spdlog")
FetchContent_Declare(
    spdlog 1.10.0
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG 76fb40d95455f249bd70824ecfcae7a8f0930fa3 # v1.10.0 | 2022-04-04
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(spdlog)

set(SPDLOG_WCHAR_TO_UTF8_SUPPORT ON CACHE BOOL "" FORCE)

# ------------------------------------------------------------------------------
# glm --------------------------------------------------------------------------
message(NOTICE "Fetch GLM")
FetchContent_Declare(
    glm 0.9.9.8
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e # 0.9.9.8 | 2020-04-13
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(glm)

# ------------------------------------------------------------------------------
# tinygltf ---------------------------------------------------------------------
message(NOTICE "Fetch TinyGLTF")
FetchContent_Declare(
    tinygltf
    GIT_REPOSITORY https://github.com/syoyo/tinygltf.git
    GIT_TAG 1a7c7d20013465b5c2a1285872b52a43ab62c1b3 # 2022-03-14
)

FetchContent_MakeAvailable(tinygltf)

set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF CACHE BOOL "" FORCE)
set(TINYGLTF_INSTALL              OFF CACHE BOOL "" FORCE)