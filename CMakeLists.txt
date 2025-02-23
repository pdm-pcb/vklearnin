cmake_minimum_required(VERSION 3.25) # Required for FetchContent SYSTEM

project(
    vklearnin
    VERSION 0.1.0
    DESCRIPTION ""
    LANGUAGES CXX C
)

# Everything depends on Vulkan
set(VKL_VK_TARGET_MAJOR 1)
set(VKL_VK_TARGET_MINOR 2)
set(
	VKL_VK_TARGET_VERSION
    "${VKL_VK_TARGET_MAJOR}.${VKL_VK_TARGET_MINOR}"
	CACHE
	STRING
    "Target Vulkan version"
	FORCE
)
add_compile_definitions(
    "VKL_VK_TARGET_MAJOR=${VKL_VK_TARGET_MAJOR}"
    "VKL_VK_TARGET_MINOR=${VKL_VK_TARGET_MINOR}"
)
find_package(Vulkan ${VKL_VK_TARGET_VERSION} REQUIRED)

# Where we keep the helpful extra CMake
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# These three functions set flags useful for both first- and third-party
include("set_target_arch")
include("add_debug_flags")
include("add_release_flags")

set_target_arch()   # Target newer CPUs, specifically AVX2
add_debug_flags()   # Disable optimizations and add debugging info
add_release_flags() # Enable optimizations and strip excess symbols

# External dependencies
include("fetch_fmt")
include("fetch_spdlog")
include("fetch_glfw3")
include("fetch_glm")
include("fetch_spirv_reflect")

fetch_fmt()
fetch_spdlog()
fetch_glfw3()
fetch_glm()
fetch_spirv_reflect()

add_subdirectory(third_party)

# Use standard C++23
set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

# Use standard C17
set(CMAKE_C_STANDARD            17)
set(CMAKE_C_STANDARD_REQUIRED   ON)
set(CMAKE_C_EXTENSIONS          OFF)

# These functions make more sense for first-party software
include("add_compiler_definitions")
include("add_warning_flags")

add_compiler_definitions()  # vklearnin specific defines
add_warning_flags()         # Many, many warnings enabled

add_subdirectory(vklearnin)