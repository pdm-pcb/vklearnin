#ifndef VKL_COMMON_HPP
#define VKL_COMMON_HPP

// #define MEMLOG
#include "vklearnin/Tools/MemTracker.hpp"

#ifdef DEBUG
    #define VK_VALIDATION_LAYER
    #include "vklearnin/Tools/VKDebugger.hpp"
#endif // DEBUG

#include "vklearnin/Tools/ConsoleLog.hpp"
#include "vklearnin/Tools/BufferTools.hpp"
#include "vklearnin/Shaders/Shader.hpp"
#include "vklearnin/Shaders/Index.hpp"
#include "vklearnin/Shaders/Vertex.hpp"
#include "vklearnin/Shaders/MVPMatrices.hpp"

#define GLM_FORCE_SWIZZLE
#define GLM_FORCE_CXX17
#define GLM_FORCE_INLINE
#define GLM_FORCE_SINGLE_ONLY
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan.h>

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

static constexpr char ENGINE_NAME[]      { "Vulkan Learnin'" };
static constexpr char APPLICATION_NAME[] { "Learnin'"        };

static constexpr uint32_t UI32MAX = std::numeric_limits<uint32_t>::max();
static constexpr uint64_t UI64MAX = std::numeric_limits<uint64_t>::max();
static constexpr uint32_t MAX_IMAGES = 2;

#endif // VKL_COMMON_HPP