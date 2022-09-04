#ifndef VKLEARNIN_PCH_HPP
#define VKLEARNIN_PCH_HPP

#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#if defined(__linux__)
    #define VK_USE_PLATFORM_XCB_KHR
#elif defined(WIN32)
    #define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <vulkan/vulkan.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_CXX17
#define GLM_FORCE_INLINE
#define GLM_FORCE_SINGLE_ONLY
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#if defined(__linux__)
    #include <xcb/xcb.h>
    #include <xcb/xcb_keysyms.h>
    #include <xcb/randr.h>

    #include <X11/keysym.h>
    #include <X11/keysymdef.h>

    #include <vulkan/vulkan_xcb.h>
#elif defined(_WIN32)
    #include <winsdkver.h>
    #include <sdkddkver.h>
    #ifndef WINVER
        #define WINVER WINVER_MAXVER
    #endif
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT _WIN32_WINNT_MAXVER
    #endif

    #define WIN32_LEAN_AND_MEAN
    #define VC_EXTRALEAN
    #define NOMINMAX

    #ifdef UNICODE
        #undef UNICODE
    #endif
    #ifdef _UNICODE
        #undef _UNICODE
    #endif

    #include <Windows.h>
    #include <Winuser.h>
    #include <CommCtrl.h>
    #include <hidusage.h>
    #include <shellscalingapi.h>

    #include <vulkan/vulkan_win32.h>
#endif

#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cassert>
#include <vector>
#include <array>
#include <limits>
#include <utility>
#include <optional>
#include <chrono>
#include <fstream>

#define VMA_VULKAN_VERSION 1001000
#include "vk_mem_alloc.h"

#endif // VKLEARNIN_PCH_HPP