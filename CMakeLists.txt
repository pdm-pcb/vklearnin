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
	find_package(Vulkan REQUIRED)
	find_package(glm REQUIRED)
	find_package(spdlog REQUIRED)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(
	vulkan-learnin
	"main.cpp"
    "MemTracker.cpp"
    "VKDebugger.cpp"
    "Instance.cpp"
    "CommandQueues.cpp"
    "Swapchain.cpp"
    "Shader.cpp"
    "Pipeline.cpp"
    "Framebuffers.cpp"
    "RenderLoop.cpp"
)

target_link_libraries(
	vulkan-learnin PUBLIC
	Vulkan::Vulkan
    glm
    spdlog
)

target_include_directories(
    vulkan-learnin PUBLIC
    "${spdlog_SOURCE_DIR}"
)

# -----------------------------------------------------------------------------
# Linux build details
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
	target_sources(
		vulkan-learnin PUBLIC
		"X11Window.cpp"
	)

	target_link_libraries(
		vulkan-learnin PUBLIC
		"xcb"
        "xcb-randr"
	)

	target_compile_options(
		vulkan-learnin PRIVATE
		-march=native -mtune=native
		-Wall -Wextra -Wsign-conversion -pedantic
        -Wno-narrowing
	)

	if(CMAKE_BUILD_TYPE MATCHES "Debug")
		target_compile_options(
			vulkan-learnin PUBLIC
			-O0 -ggdb3
		)

        target_compile_definitions(
            vulkan-learnin PUBLIC
            "DEBUG"
        )
	elseif(CMAKE_BUILD_TYPE MATCHES "Release")
		target_compile_options(
			vulkan-learnin PUBLIC
			-O3
		)
	endif()

# -----------------------------------------------------------------------------
# Windows build details
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
	target_sources(
		vulkan-learnin PUBLIC
		"Win32Window.cpp"
	)

	target_compile_options(
		vulkan-learnin PUBLIC
		"/nologo"  # Suppress copyright message
		"/W4"	   # Warnin's
		"/wd4312"  # Disable the warning for casting to a bigger size
	)

	target_compile_definitions(
		vulkan-learnin PUBLIC
		"WIN64"
	)

	if(CMAKE_BUILD_TYPE MATCHES "Debug")
		target_compile_options(
			vulkan-learnin PUBLIC
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
            vulkan-learnin PUBLIC
            "DEBUG"
        )
	elseif(CMAKE_BUILD_TYPE MATCHES "Release")
		target_compile_options(
			vulkan-learnin PUBLIC
			"/O2"         # Optimization not disabled =D
			"/fp:fast"    # Gotta go fast
			"/GS-"        # No buffer overrun checks
		)
	endif()
endif()

set_target_properties(
	vulkan-learnin PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
    PCH_WARN_INVALID OFF
	EXECUTABLE_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/debug/lib
	EXECUTABLE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/release/lib
)

get_target_property(vulkan-learnin_flags vulkan-learnin COMPILE_OPTIONS)
get_target_property(vulkan-learnin_defs  vulkan-learnin COMPILE_DEFINITIONS)
get_target_property(vulkan-learnin_inc   vulkan-learnin INCLUDE_DIRECTORIES)
get_target_property(vulkan-learnin_link  vulkan-learnin LINK_DIRECTORIES)

message(STATUS "vulkan-learnin compile flags: ${vulkan-learnin_flags}")
message(STATUS "vulkan-learnin compile defines: ${vulkan-learnin_defs}")
message(STATUS "vulkan-learnin include directories: ${vulkan-learnin_inc}")
message(STATUS "vulkan-learnin link directories: ${vulkan-learnin_link}")
message(STATUS "cmake compile flags: ${CMAKE_CXX_FLAGS}")
 
