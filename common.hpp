#ifndef VKL_COMMON_HPP
#define VKL_COMMON_HPP

static constexpr char ENGINE_NAME[]      { "Vulkan Learnin'" };
static constexpr char APPLICATION_NAME[] { "Learnin'"        };

#include "MemTracker.hpp"
#include "ConsoleLog.hpp"

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>

#if defined(__linux__)
    #include <xcb/xcb.h>
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
#include <cassert>
#include <cstdio>
#include <vector>
#include <limits>
#include <utility>

static constexpr uint32_t UI32MAX = std::numeric_limits<uint32_t>::max();
static constexpr uint64_t UI64MAX = std::numeric_limits<uint64_t>::max();

#endif // VKL_COMMON_HPP