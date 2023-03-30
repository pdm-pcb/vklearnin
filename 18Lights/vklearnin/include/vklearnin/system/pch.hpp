#ifndef VKLEARNIN_SYSTEM_PCH_HPP
#define VKLEARNIN_SYSTEM_PCH_HPP

// Allow for the use of designated initializers
// https://github.com/KhronosGroup/Vulkan-Hpp#designated-initializers
#define VULKAN_HPP_NO_CONSTRUCTORS

// Prevent needing to load every function by hand
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <vulkan/vulkan.hpp>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_vulkan.h>

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

#include <csignal>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <list>
#include <set>

#include "vklearnin/math/math.hpp"
#include "vklearnin/math/Color.hpp"

#endif // VKLEARNIN_SYSTEM_PCH_HPP