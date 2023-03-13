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

// Platform specific WSI defines
#if defined(VKL_LINUX)
    #define VK_USE_PLATFORM_XCB_KHR
#elif defined(VKL_WINDOWS)
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <vulkan/vulkan.hpp>

#if defined(VKL_LINUX)
    #include <X11/Xlib.h>
    #include <vulkan/vulkan_xlib.h>
#elif defined(VKL_WINDOWS)
    #include <Windows.h>
    #undef min // Defining NOMINMAX above doesn't give the desired results with
    #undef max // how I've got things set up. This is a blunt approach, but
               // but nobody should be globally defining 'min' and 'max' in the
               // first place.
    // Grr...
    #undef near
    #undef far

    #include <vulkan/vulkan_win32.h>
#endif

#include "vklearnin/math/math.hpp"

#include <filesystem>
#include <fstream>
#include <set>
#include <numbers>
#include <list>
#include <type_traits>
#include <system_error>
#include <unordered_map>

#endif // VKLEARNIN_SYSTEM_PCH_HPP