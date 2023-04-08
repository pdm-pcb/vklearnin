#ifndef VKLEARNIN_VKLEARNIN_HPP
#define VKLEARNIN_VKLEARNIN_HPP

// #define MEMLOG
#include "vklearnin/tools/MemTracker.hpp"
#include "vklearnin/tools/ConsoleLog.hpp"
#include "vklearnin/tools/Timekeeper.hpp"

#include "vklearnin/system/Application.hpp"
#include "vklearnin/system/events/Events.hpp"
#include "vklearnin/events/EventBroker.hpp"
#include "vklearnin/rendering/RenderConfig.hpp"
#include "vklearnin/rendering/Renderer.hpp"

#include "vklearnin/resources/images/ImageTools.hpp"
#include "vklearnin/resources/buffers/BufferTools.hpp"

namespace vkl {

// These constants help Vulkan advertise the application/engine to the drivers,
// in case there are any specific optimizations
static char     constexpr APP_NAME[]    { "vklearnin: Demo" };
static char     constexpr ENGINE_NAME[] { "vklearnin" };
static uint32_t constexpr APP_VERSION    = VK_MAKE_API_VERSION(0, 0, 1, 0);
static uint32_t constexpr ENGINE_VERSION = VK_MAKE_API_VERSION(0, 0, 1, 0);

static uint32_t constexpr VK_API_VER = VK_API_VERSION_1_3;

#if VKL_DEBUG
static std::filesystem::path const ASSET_PATH("../../assets");
static std::string           const SHADER_EXT("-debug.spv");
#else
static std::filesystem::path const ASSET_PATH("assets");
static std::string           const SHADER_EXT("-release.spv");
#endif

} // namespace vkl

#endif // VKLEARNIN_VKLEARNIN_HPP