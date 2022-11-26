list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/")
include(FetchDeps)
include(CompileShaders)

find_package(Vulkan REQUIRED)

file(
    GLOB_RECURSE PROJECT_SOURCE
    "src/*.cpp"
)

file(
    GLOB_RECURSE PROJECT_HEADERS
    "include/vklearnin/*.hpp"
)

add_library(
	vklearnin STATIC
	${PROJECT_SOURCE}
    ${PROJECT_HEADERS}
)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_definitions(
		vklearnin PUBLIC
		"VKL_DEBUG"
	)
endif()

target_compile_options(
    vklearnin PUBLIC
    "/nologo"  # Suppress copyright message
    "/W4"	   # Warnin's
    "/wd4312"  # Disable the warning for casting to a bigger size
)

target_include_directories(
    vklearnin PUBLIC
    "include/"
)

target_include_directories(
    vklearnin SYSTEM PUBLIC
    "${spdlog_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/third_party/"
)

target_precompile_headers(
    vklearnin PUBLIC
    "include/vklearnin/system/pch.hpp"
)

target_link_libraries(
	vklearnin PUBLIC
	Vulkan::Vulkan
    spdlog
    glm
    third_party
)

file(
    GLOB SHADER_SOURCE
    "${CMAKE_CURRENT_LIST_DIR}/assets/shaders/*.vert"
    "${CMAKE_CURRENT_LIST_DIR}/assets/shaders/*.frag"
)

if("${SHADER_SOURCE}" STREQUAL "")
    message(FATAL_ERROR "Shader source glob was empty")
endif()

compile_shader(
    vklearnin
    ENV vulkan1.2
    SOURCES ${SHADER_SOURCE}
)

set_target_properties(
	vklearnin PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
    PCH_WARN_INVALID OFF
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG      ${PROJECT_SOURCE_DIR}/debug/lib
	LIBRARY_OUTPUT_DIRECTORY_DEBUG      ${PROJECT_SOURCE_DIR}/debug/lib
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE    ${PROJECT_SOURCE_DIR}/release/lib
	LIBRARY_OUTPUT_DIRECTORY_RELEASE    ${PROJECT_SOURCE_DIR}/release/lib
)