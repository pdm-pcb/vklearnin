include(FetchContent)

# ------------------------------------------------------------------------------
# glm --------------------------------------------------------------------------
message(NOTICE "Fetch VMA")
FetchContent_Declare(
    vma 3.0.1
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
    GIT_TAG a6bfc237255a6bac1513f7c1ebde6d8aed6b5191 # v3.0.1 | 2022-05-26
)
FetchContent_MakeAvailable(vma)

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
# spdlog -----------------------------------------------------------------------
message(NOTICE "Fetch spdlog")
FetchContent_Declare(
    spdlog 1.10.0
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG 76fb40d95455f249bd70824ecfcae7a8f0930fa3 # v1.10.0 | 2022-04-04
)
FetchContent_MakeAvailable(spdlog)

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

target_compile_definitions(
    tinygltf PRIVATE
    "TINYGLTF_NOEXCEPTION"
    "JSON_NOEXCEPTION"
    "TINYGLTF_USE_CPP14"
)

set_target_properties(
    tinygltf PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)