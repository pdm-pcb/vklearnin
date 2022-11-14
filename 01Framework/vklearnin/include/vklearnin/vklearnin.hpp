#ifndef VKLEARNIN_VKLEARNIN_HPP
#define VKLEARNIN_VKLEARNIN_HPP

//#define MEMLOG
#include "vklearnin/MemTracker.hpp"
#include "vklearnin/ConsoleLog.hpp"
#include "vklearnin/Application.hpp"

// These constants help Vulkan advertise the application/engine to the drivers,
// in case there are any specific optimizations
static constexpr char APP_NAME[] { "vklearnin: Demo" };
static constexpr char ENGINE_NAME[] { "vklearnin" };
static constexpr uint32_t APP_VERSION    = VK_MAKE_API_VERSION(0, 0, 1, 0);
static constexpr uint32_t ENGINE_VERSION = VK_MAKE_API_VERSION(0, 0, 1, 0);

// Vulkan 1.1 has everything we need
static constexpr uint32_t VK_API_VER = VK_API_VERSION_1_1;

#endif // VKLEARNIN_VKLEARNIN_HPP