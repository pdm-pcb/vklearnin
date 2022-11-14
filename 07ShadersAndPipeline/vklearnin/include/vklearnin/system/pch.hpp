#ifndef VKLEARNIN_SYSTEM_PCH_HPP
#define VKLEARNIN_SYSTEM_PCH_HPP

// Designated initializers
// https://github.com/KhronosGroup/Vulkan-Hpp#designated-initializers
#define VULKAN_HPP_NO_CONSTRUCTORS

// No need for exceptions - I'd rather check the return values
#define VULKAN_HPP_NO_EXCEPTIONS

// Prevent needing to load every function by hand
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

// Platform specific WSI defines
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
    #define WIN32_LEAN_AND_MEAN // probably doesn't do much nowadays
    #include <Windows.h>
    #undef min // so we can use std::numeric_limits
    #undef max // <--^

    #include <vulkan/vulkan_win32.h>
#endif

#include <cstdint>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <limits>
#include <set>
#include <fstream>

#endif // VKLEARNIN_SYSTEM_PCH_HPP