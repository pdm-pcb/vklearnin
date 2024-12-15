#ifndef VKLEARNIN_PCH_HPP
#define VKLEARNIN_PCH_HPP

#include <filesystem>
#include <span>
#include <fstream>
#include <bitset>
#include <chrono>
#include <random>
#include <numbers>

#define VULKAN_HPP_NO_CONSTRUCTORS           // Use designated initializers
#define VULKAN_HPP_NO_SETTERS                // structs don't need setters
#define VULKAN_HPP_NO_EXCEPTIONS             // Check the return values manually
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1 // Find function pointers for us

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
