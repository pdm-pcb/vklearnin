if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_compile_definitions("DEBUG")
endif()

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

target_include_directories(
    vklearnin PUBLIC
    "include/"
)

target_precompile_headers(
    vklearnin PUBLIC
    "include/vklearnin/common.hpp"
)

target_include_directories(
    vklearnin PUBLIC
    "${fmt_SOURCE_DIR}"
    "${spdlog_SOURCE_DIR}"
    "${glm_SOURCE_DIR}"
)

target_link_libraries(
	vklearnin PUBLIC
	Vulkan::Vulkan
    fmt
    spdlog
    third_party
)

set(
    SHADER_SOURCE
    "${PROJECT_SOURCE_DIR}/assets/shaders/shader.vert"
    "${PROJECT_SOURCE_DIR}/assets/shaders/shader.frag"
)

if("${SHADER_SOURCE}" STREQUAL "")
    message(FATAL_ERROR "Shader source glob was empty")
endif()

compile_shader(
    vklearnin
    ENV vulkan1.3
    SOURCES ${SHADER_SOURCE}
)

# -----------------------------------------------------------------------------
# Linux build details
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(
		vklearnin PUBLIC
		"xcb"
        "xcb-keysyms"
        "xcb-randr"
	)

	target_compile_options(
		vklearnin PRIVATE
		-march=native -mtune=native
		-Wall -Wextra -pedantic -Wsign-conversion
	)

	if(CMAKE_BUILD_TYPE MATCHES "Debug")
		target_compile_options(
			vklearnin PUBLIC
			-O0 -ggdb3
		)
        target_compile_definitions(
            vklearnin PUBLIC
            "DEBUG"
        )
	elseif(CMAKE_BUILD_TYPE MATCHES "Release")
		target_compile_options(
			vklearnin PUBLIC
			-O3
		)
	endif()

# -----------------------------------------------------------------------------
# Windows build details
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
	target_compile_options(
		vklearnin PUBLIC
		"/nologo"  # Suppress copyright message
		"/W4"	   # Warnin's
		"/wd4312"  # Disable the warning for casting to a bigger size
	)

	target_compile_definitions(
		vklearnin PUBLIC
		"WIN64"
	)

	if(CMAKE_BUILD_TYPE MATCHES "Debug")
		target_compile_options(
			vklearnin PUBLIC
			"/JMC"      # Just-My-Code
			"/Zi"		# Debugging information
			"/RTC1"     # Runtime checks
			"/GS"       # Buffer overrun checks
			"/sdl"      # Security warnings
			"/Od"       # Optimization disabled
			"/EHsc"     # Standard exception handling
			"/diagnostics:column" # Include column number in compiler messages
        )

        target_compile_definitions(
            vklearnin PUBLIC
            "DEBUG"
        )
	elseif(CMAKE_BUILD_TYPE MATCHES "Release")
		target_compile_options(
			vklearnin PUBLIC
			"/O2"         # Optimization not disabled =D
			"/fp:fast"    # Gotta go fast
			"/GS-"        # No buffer overrun checks
		)
	endif()
endif()

set_target_properties(
	vklearnin PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
    PCH_WARN_INVALID OFF
	EXECUTABLE_OUTPUT_DIRECTORY_DEBUG   ${PROJECT_SOURCE_DIR}/debug/bin
	EXECUTABLE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/release/bin
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG      ${PROJECT_SOURCE_DIR}/debug/lib
	LIBRARY_OUTPUT_DIRECTORY_DEBUG      ${PROJECT_SOURCE_DIR}/debug/lib
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE    ${PROJECT_SOURCE_DIR}/release/lib
	LIBRARY_OUTPUT_DIRECTORY_RELEASE    ${PROJECT_SOURCE_DIR}/release/lib
)

get_target_property(vklearnin_flags vklearnin COMPILE_OPTIONS)
get_target_property(vklearnin_defs  vklearnin COMPILE_DEFINITIONS)
get_target_property(vklearnin_inc   vklearnin INCLUDE_DIRECTORIES)
get_target_property(vklearnin_link  vklearnin LINK_DIRECTORIES)

message(NOTICE "vklearnin compile flags: ${vklearnin_flags}")
message(NOTICE "vklearnin compile defines: ${vklearnin_defs}")
message(NOTICE "vklearnin include directories: ${vklearnin_inc}")
message(NOTICE "vklearnin link directories: ${vklearnin_link}")
message(NOTICE "cmake compile flags: ${CMAKE_CXX_FLAGS}")