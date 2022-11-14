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

target_precompile_headers(
    vklearnin PUBLIC
    "include/vklearnin/pch.hpp"
)

target_link_libraries(
	vklearnin PUBLIC
	Vulkan::Vulkan
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