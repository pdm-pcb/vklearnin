#ifndef VKLEARNIN_SYSTEM_PCH_HPP
#define VKLEARNIN_SYSTEM_PCH_HPP

// Designated initializers
// https://github.com/KhronosGroup/Vulkan-Hpp#designated-initializers
#define VULKAN_HPP_NO_CONSTRUCTORS

#define VULKAN_HPP_NO_EXCEPTIONS    // I perfer to check the return values
#define VULKAN_HPP_ASSERT_ON_RESULT // Assert on result can trip things up when
                                    // exceptions are disabled

// Prevent needing to load every function by hand
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#if defined(VKL_LINUX)
    #include <xcb/xcb.h>
    #include <xcb/xcb_keysyms.h>
    #include <xcb/randr.h>

    #include <X11/keysym.h>
    #include <X11/keysymdef.h>
    #include <X11/Xlib-xcb.h>

    #define VK_USE_PLATFORM_XCB_KHR
    #include <vulkan/vulkan.hpp>
    #include <vulkan/vulkan_xcb.h>
#elif defined(VKL_WINDOWS)
    #define NOMINMAX
    #include <Windows.h>

    #define VK_USE_PLATFORM_WIN32_KHR
    #include <vulkan/vulkan.hpp>
    #include <vulkan/vulkan_win32.h>
#endif

#include <filesystem>
#include <fstream>
#include <set>
#include <numbers>
#include <unordered_map>

#endif // VKLEARNIN_SYSTEM_PCH_HPP