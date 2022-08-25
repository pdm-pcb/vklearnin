cmake_minimum_required(VERSION 3.24)

project(
    "Vulkan Learnin'"
    VERSION 0.0.1
    LANGUAGES CXX C
)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_compile_definitions("DEBUG")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

    include(CompileShaders)

	find_package(Vulkan REQUIRED)
	find_package(glm REQUIRED)
	find_package(spdlog REQUIRED)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(
    GLOB_RECURSE PROJECT_SOURCE
    "src/*.cpp"
)

file(
    GLOB_RECURSE PROJECT_HEADERS
    "include/vklearnin/*.hpp"
)

add_executable(
	vklearnin
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

file(
    GLOB SHADER_SOURCE
    "shaders/shader.vert"
    "shaders/shader.frag"
)

compile_shader(
    vklearnin
    ENV vulkan1.3
    SOURCES ${SHADER_SOURCE}
)

target_link_libraries(
	vklearnin PUBLIC
	Vulkan::Vulkan
    glm
    spdlog
)

target_include_directories(
    vklearnin PUBLIC
    "${spdlog_SOURCE_DIR}"
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
	EXECUTABLE_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/debug/lib
	EXECUTABLE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/release/lib
)

get_target_property(vklearnin_flags vklearnin COMPILE_OPTIONS)
get_target_property(vklearnin_defs  vklearnin COMPILE_DEFINITIONS)
get_target_property(vklearnin_inc   vklearnin INCLUDE_DIRECTORIES)
get_target_property(vklearnin_link  vklearnin LINK_DIRECTORIES)

message(STATUS "vklearnin compile flags: ${vklearnin_flags}")
message(STATUS "vklearnin compile defines: ${vklearnin_defs}")
message(STATUS "vklearnin include directories: ${vklearnin_inc}")
message(STATUS "vklearnin link directories: ${vklearnin_link}")
message(STATUS "cmake compile flags: ${CMAKE_CXX_FLAGS}")
