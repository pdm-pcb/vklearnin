#ifndef VKLEARNIN_PCH_HPP
#define VKLEARNIN_PCH_HPP

#include <filesystem>
#include <span>
#include <fstream>
#include <bitset>
#include <chrono>
#include <random>
#include <numbers>

// Allow for the use of designated initializers
// https://github.com/KhronosGroup/Vulkan-Hpp#designated-initializers
#define VULKAN_HPP_NO_CONSTRUCTORS

// Check the return values manually
// https://github.com/KhronosGroup/Vulkan-Hpp#return-values-error-codes--exceptions
#define VULKAN_HPP_NO_EXCEPTIONS

// No need to load every function by hand
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#ifdef VKL_LINUX
    #define VK_USE_PLATFORM_XLIB_KHR
    #include <vulkan/vulkan.hpp>
    #include <vulkan/vulkan_xlib.h>
#elif VKL_WINDOWS
    #define NOMINMAX

    #define VK_USE_PLATFORM_WIN32_KHR
    #include <vulkan/vulkan.hpp>
    #include <vulkan/vulkan_win32.h>
#endif // BTX platform

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include <spirv_reflect.h>

// These don't need to be built by every translation unit
#include "vklearnin/version.hpp"
#include "vklearnin/tools/Log.hpp"
#include "vklearnin/vulkan/type_formatters.hpp"
#include "vklearnin/platform/vkl_keycodes.hpp"
#include "vklearnin/platform/GLFWtoVKLKeys.hpp"

#endif // VKLEARNIN_PCH_HPP
