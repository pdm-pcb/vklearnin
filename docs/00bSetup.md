## Project Structure
Here's the directory hierarchy I'll be using for the examples from here on out. As more files get added, I'll add subdirectories and adjust as necessary. For now though, let's get something up and running.

The plan here is that your rendering engine will be a static library while you'll develop a demo application separately as a means of proving out the library. To that end:

```
project_root/
	demo/
		include/
			Demo.hpp
		src/
			Demo.cpp
			main.cpp
		CMakeLists.txt
	vklearnin/
		cmake/
			FetchDeps.cmake
		include/vklearnin/
			system/
				Application.hpp
				Engine.hpp
				pch.hpp
			tools/
				ConsoleLog.hpp
				MemTracker.hpp
				Timekeeper.hpp
			vklearnin.hpp
		src/
			system/
				Application.cpp
				Engine.cpp
			tools/
				ConsoleLog.cpp
				Timekeeper.cpp
		CMakeLists.txt
	CMakeLists.txt
```

## Initial CMake Files
Starting at the root folder, here's `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.19)

project(
    vklearnin
    VERSION 0.1.0
    DESCRIPTION "Exploring real time graphics."
    LANGUAGES CXX C
)

add_subdirectory(vklearnin)
add_subdirectory(demo)
```

Feel free to adapt as you see fit. CMake 3.19 is required so we can import the [`Vulkan::glslc`](https://cmake.org/cmake/help/latest/module/FindVulkan.html) target in later chapters. 

The `CMakeListst.txt` file in `demo/` will look like this:

```cmake
file(
    GLOB_RECURSE DEMO_SOURCE
    "src/*.cpp"
)

file(
    GLOB_RECURSE DEMO_HEADERS
    "include/*.hpp"
)

add_executable(
    demo
	${DEMO_SOURCE}
    ${DEMO_HEADERS}
)

target_include_directories(
	demo PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
	demo PUBLIC
	vklearnin
)

set_target_properties(
	demo PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/debug/bin
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/release/bin
)
```

And  `vklearnin/CMakeLists.txt` will be a bit lengthier. If you only plan on targeting one platform or toolchain, feel free to drop the fluff I've got for accommodating multiple.

```cmake
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/")
include(FetchDeps)

find_package(Vulkan REQUIRED)

file(
    GLOB_RECURSE PROJECT_SOURCE
    "src/*.cpp"
)

file(
    GLOB_RECURSE PROJECT_HEADERS
    "include/vklearnin/*.hpp"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
    "include/vklearnin/system/pch.hpp"
)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_definitions(
		vklearnin PUBLIC
		"VKL_DEBUG"
	)
endif()

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    target_compile_definitions(
        vklearnin PUBLIC
        "VKL_LINUX"
    )

    target_link_libraries(
        vklearnin PUBLIC
        "xcb"
        "xcb-keysyms"
        "xcb-randr"
        "X11-xcb"
    )

    target_compile_options(
        vklearnin PUBLIC
        "-Wall"
        "-Wextra"
        "-pedantic"
    )

    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        target_compile_options(
            vklearnin PUBLIC
            "-O0"
            "-ggdb3"
        )
    elseif(CMAKE_BUILD_TYPE MATCHES "Release")
        target_compile_options(
            vklearnin PUBLIC
            "-O3"
            "-ffast-math"
        )
    endif()
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    target_compile_definitions(
        vklearnin PUBLIC
        "VKL_WINDOWS"
    )

    if(CMAKE_C_COMPILER MATCHES "clang")
        target_compile_options(
            vklearnin PUBLIC
            "-Wall"
            "-Wextra"
            "-pedantic"
        )

        if(CMAKE_BUILD_TYPE MATCHES "Debug")
            target_compile_options(
                vklearnin PUBLIC
                "-O0"
                "-ggdb3"
            )
        elseif(CMAKE_BUILD_TYPE MATCHES "Release")
            target_compile_options(
                vklearnin PUBLIC
                "-O3"
                "-ffast-math"
            )
        endif()
    else()
        target_compile_definitions(
            vklearnin PUBLIC
            "_CRT_SECURE_NO_WARNINGS" # ignore "may be unsafe" warnings
            "WIN64" # shun the 32 bit address space
        )

        target_compile_options(
            vklearnin PUBLIC
            "/nologo"  # Suppress copyright message
            "/W4"	   # Warnin's
            "/wd4312"  # Disable the warning for casting to a larger size
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
        elseif(CMAKE_BUILD_TYPE MATCHES "Release")
            target_compile_options(
                vklearnin PUBLIC
                "/O2"         # Optimization not disabled =D
                "/fp:fast"    # Gotta go fast
                "/GS-"        # No buffer overrun checks
            )
        endif()
    endif()
endif()

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
```

## Static Library
etc etc

## Demo Application
etc etc
