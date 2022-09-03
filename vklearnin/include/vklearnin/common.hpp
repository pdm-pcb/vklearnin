#ifndef VKLEARNIN_COMMON_HPP
#define VKLEARNIN_COMMON_HPP

#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#if defined(__linux__)
    #define VK_USE_PLATFORM_XCB_KHR
#elif defined(WIN32)
    #define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <vulkan/vulkan.hpp>

// #define MEMLOG
#include "vklearnin/Tools/MemTracker.hpp"

#ifdef DEBUG
    #include "vklearnin/Tools/VKDebugger.hpp"
#endif // DEBUG

#include "vklearnin/Tools/ConsoleLog.hpp"
#include "vklearnin/Tools/Allocator.hpp"
#include "vklearnin/Tools/BufferTools.hpp"
#include "vklearnin/Tools/ImageTools.hpp"
#include "vklearnin/Shaders/Shader.hpp"
#include "vklearnin/Shaders/Index.hpp"
#include "vklearnin/Shaders/Vertex.hpp"
#include "vklearnin/Shaders/MVPMatrices.hpp"

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

static constexpr char APP_NAME[] { "Learnin'" };
static constexpr char ENGINE_NAME[] { "Vulkan Learnin'" };
static constexpr uint32_t APP_VERSION = VK_MAKE_API_VERSION(0, 0, 1, 0);
static constexpr uint32_t ENGINE_VERSION = VK_MAKE_API_VERSION(0, 0, 1, 0);

static constexpr uint32_t UI32MAX = std::numeric_limits<uint32_t>::max();
static constexpr uint64_t UI64MAX = std::numeric_limits<uint64_t>::max();
static constexpr uint32_t FRAME_OVERLAP = 2;

static constexpr uint32_t VK_API_VER = VK_API_VERSION_1_1;

#define VMA_VULKAN_VERSION 1001000
#include "vk_mem_alloc.h"

#endif // VKLEARNIN_COMMON_HPP