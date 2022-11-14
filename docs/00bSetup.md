## Project Structure
Here's the directory heirarchy I'll be using for the examples from here on out. As more files get added, I'll add subdirectories and adjust as necessary. For now though, let's get something up and running.

The plan here is that your rendering engine will be a static library while you'll develop a demo application separately as a means of proving out the library. To that end:
```
project_root/
	.vscode/
		launch.json
	demo/
		include/
			Demo.hpp
		src/
			main.cpp
		CMakeLists.txt
	vklearnin
		include/vklearnin/
			Application.hpp
			pch.hpp
			vklearnin.hpp
		src/
			Application.cpp
			MemTracker.cpp
		CMakeLists.txt
	CMakeLists.txt
	CMakePresets.json
```

## Initial CMake Files
Starting at the root folder, here's `CMakeLists.txt`:
```cmake
cmake_minimum_required(VERSION 3.19)

project(
    vklearnin
    VERSION 0.1.0
    DESCRIPTION "A tutorial."
    LANGUAGES CXX C
)

add_subdirectory(vklearnin)
add_subdirectory(demo)
```

Feel free to adapt as you see fit. CMake 3.19 is required so we can import the `Vulkan::glslc` target in later chapters. Moving on to `CMakePresets.json`:
```json
{
    "version": 3,
    "cmakeMinimumRequired": {
        "major": 3,
        "minor": 19,
        "patch": 0
    },
    "configurePresets": [
        {
            "name": "base",
            "generator": "Ninja",
            "hidden": true,
            "binaryDir": "${sourceDir}/build/${presetName}"
        },
        {
            "name": "linux-base",
            "hidden": true,
            "inherits": "base"
        },
        {
            "name": "linux-debug",
            "condition": {
                "type": "equals",
                "lhs": "${hostSystemName}",
                "rhs": "Linux"
            },
            "displayName": "Linux Debug",
            "inherits": "base",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Debug"
            }
        },
        {
            "name": "linux-release",
            "condition": {
                "type": "equals",
                "lhs": "${hostSystemName}",
                "rhs": "Linux"
            },
            "displayName": "Linux Release",
            "inherits": "linux-base",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Release"
            }
        },
        {
            "name": "windows-base",
            "hidden": true,
            "inherits": "base",
            "architecture": {
                "value": "x64",
                "strategy": "external"
            },
            "toolset": {
                "value": "host=x64",
                "strategy": "external"
            },
            "cacheVariables": {
                "CMAKE_C_COMPILER": "cl",
                "CMAKE_CXX_COMPILER": "cl"
            },
            "vendor": {
                "microsoft.com/VisualStudioSettings/CMake/1.0": {
                    "hostOS": [
                        "Windows"
                    ]
                }
            }
        },
        {
            "name": "windows-debug-vulkan",
            "condition": {
                "type": "equals",
                "lhs": "${hostSystemName}",
                "rhs": "Windows"
            },
            "displayName": "Windows Vulkan Debug",
            "inherits": "windows-base",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Debug"
            }
        },
        {
            "name": "windows-release-vulkan",
            "condition": {
                "type": "equals",
                "lhs": "${hostSystemName}",
                "rhs": "Windows"
            },
            "displayName": "Windows Vulkan Release",
            "inherits": "windows-base",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Release"
            }
        }
    ]
}
```

That's quite a lot. If you want to take a look at the official [documentation](https://learn.microsoft.com/en-us/cpp/build/cmake-presets-vs?view=msvc-170) (according to Microsoft) have at it. I'll briefly summarize what I've got, too.

Declaring `CMakePresets.json`'s `"version"` field to `3` keeps VSCode's integrated CMake tools happy. Different versions of the toolset might support different versions of the schema, so read up on whatever you're using.

The first two entries in `"configurePresets"` are hidden from which the next few can inherit, reducing a tiny bit of duplicate configuration. Then, Linux and Windows both get fleshed-out Debug and Release configurations. It's a lot of scribbling, but it'll always be the same for these tutorials and it makes cross platform development considerably easier to stomach.

Next is the `.vscode` directory with its one file, `launch.json`. This will differ based on platform. Having the file is a benefit though, as it lets you use F5/F7 plainly instead of relying on the CMake plugin's Ctrl+F5 shortcut and so forth.  For Windows, use the following:

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "(msvc) Launch",
            "type": "cppvsdbg",
            "request": "launch",
            "program": "${command:cmake.launchTargetPath}",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${command:cmake.getLaunchTargetDirectory}",
            "console": "externalTerminal",
        },
        {
            "name": "(gdb) Launch",
            "type": "cppdbg",
            "request": "launch",
            "program": "${command:cmake.launchTargetPath}",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${command:cmake.getLaunchTargetDirectory}",
            "externalConsole": true,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        }
    ]
}
```

For Linux, just swap the order of the two entries.

## Demo Application
On to the `demo` folder. This will contain next to nothing until we're ready for application-specific assets and the like. For now, `demo/include/Demo.hpp` can look like this:
```cpp
#ifndef DEMO_HPP
#define DEMO_HPP

#include "vklearnin/vklearnin.hpp"

class Demo : public vkl::Application {

};

#endif // DEMO_HPP
```

And `demo/src/main.cpp` offers the following:
```cpp
#include "vklearnin/vklearnin.hpp"
#include "Demo.hpp"

int main() {
    vkl::Application *app = new Demo;
    app->init();
    app->run();
    delete app;

    return 0;
```

Tying them both together with the build system is `demo/CMakeLists.txt`:
```CMake
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

## Static Library
Finally, it's time for the bones of the project where we'll spend most of our effort. `vklearnin/include/vklearnin/Application.hpp` is the base class declaration for the `Demo` class above.
```cpp
#ifndef VKLEARNIN_APPLICATION_HPP
#define VKLEARNIN_APPLICATION_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

class Application {
public:
    void init();
    void run();

    Application();
    ~Application();

    Application(Application &&) = delete;
    Application(const Application &) = delete;

    Application & operator=(Application &&) = delete;
    Application & operator=(const Application &) = delete;
};

} // namespace vkl
#endif // VKLEARNIN_APPLICATION_HPP
```

In an effort to keep compile times under control, we'll use a precompiled header for the standard library, STL, and anything else unlikely to change with any regularity. It’ll also include `vulkan.hpp`, with a handful of tweaks. Create `vklearnin/include/vklearnin/pch.hpp` and populate it with the following:
```cpp
#ifndef VKLEARNIN_PCH_HPP
#define VKLEARNIN_PCH_HPP

#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#if defined(__linux__)
    #define VK_USE_PLATFORM_XCB_KHR
#elif defined(WIN32)
    #define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <vulkan/vulkan.hpp>

#if defined(__linux__)
    #include <xcb/xcb.h>
    #include <xcb/xcb_keysyms.h>
    #include <xcb/randr.h>

    #include <X11/keysym.h>
    #include <X11/keysymdef.h>
    #include <X11/Xlib-xcb.h>

    #include <vulkan/vulkan_xcb.h>
#elif defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>

    #include <vulkan/vulkan_win32.h>
#endif

#include <cstdint>

#endif // VKLEARNIN_PCH_HPP
```

Disabling constructors permits the use of [designated initializer lists](https://github.com/KhronosGroup/Vulkan-Hpp#designated-initializers) with C++20. Disabling exceptions is self-explanatory. The dynamic dispatch loader is a handy feature which simply grabs all known function pointers and populates them for us. There’s an argument against this in the name of efficiency, but it’s a tradeoff I find worthwhile in this context.

One more preprocessor definition lets the main Vulkan header what platform we’re on. Below that, include the header corresponding to your operating system and platform requirements.

As a final include, I’ve thrown in `cstdint` for access to the [fixed width integer types](https://en.cppreference.com/w/cpp/types/integer).

Next up is the catchall header for the library, `vklearnin/include/vklearnin/vklearnin.hpp`:
```cpp
#ifndef VKLEARNIN_VKLEARNIN_HPP
#define VKLEARNIN_VKLEARNIN_HPP

#include "vklearnin/Application.hpp"

static constexpr char APP_NAME[] { "vklearnin: Demo" };
static constexpr char ENGINE_NAME[] { "vklearnin" };
static constexpr uint32_t APP_VERSION    = VK_MAKE_API_VERSION(0, 0, 1, 0);
static constexpr uint32_t ENGINE_VERSION = VK_MAKE_API_VERSION(0, 0, 1, 0);

static constexpr uint32_t VK_API_VER = VK_API_VERSION_1_1;

#endif // VKLEARNIN_VKLEARNIN_HPP
```

The two constant strings and two version numbers will help Vulkan advertise your application to the drivers. Admittedly, this is more of a formality for anyone rolling their own engine. It’s nice to be thorough, though. The last variable is a nice little centralized value representing the version of Vulkan we’ll request from the drivers.

Why are we working with only Vulkan 1.1 when newer versions are widely supported? Simply put: nothing in this tutorial uses anything beyond the 1.1 feature set.

The `Application` class will need  a definition, though a stubby one for now. Add `vklearnin/src/Application.hpp` and slap this in there.
```cpp
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/Application.hpp"

namespace vkl {

void Application::init() {
}

void Application::run() {
}

Application::Application() {
}

Application::~Application() {
}

} // namespace vkl
```

Finally, as with `demo/`, let's bring it all together with `vklearnin/vklearnin/CMakeLists.txt`.
```cmake
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
```

## Test Run
With all of that in place, you should be able to open the project directory with VSCode, select your platform's Debug preset and hit F5. If you the program compiles and runs (though providing zero output) it's time to move on to the first tools the project will use all over the place.