#ifndef VKLEARNIN_VKLEARNIN_HPP
#define VKLEARNIN_VKLEARNIN_HPP

// #define MEMLOG
#include "vklearnin/tools/MemTracker.hpp"
#include "vklearnin/tools/ConsoleLog.hpp"
#include "vklearnin/tools/Timekeeper.hpp"

#include "vklearnin/system/Application.hpp"
#include "vklearnin/rendering/RenderConfig.hpp"

#include "vklearnin/system/GraphicsAPI.hpp"
#include "vklearnin/rendering/Renderer.hpp"

#include "vklearnin/resources/images/ImageTools.hpp"
#include "vklearnin/resources/buffers/BufferTools.hpp"

#include "vklearnin/math/math.hpp"

namespace vkl {

// These constants help Vulkan advertise the application/engine to the drivers,
// in case there are any specific optimizations
static constexpr char APP_NAME[] { "vklearnin: Demo" };
static constexpr char ENGINE_NAME[] { "vklearnin" };
static constexpr uint32_t APP_VERSION = VK_MAKE_API_VERSION(0, 0, 1, 0);
static constexpr uint32_t ENGINE_VERSION = VK_MAKE_API_VERSION(0, 0, 1, 0);

static constexpr uint32_t VK_API_VER = VK_API_VERSION_1_2;

#if VKL_DEBUG
static const std::filesystem::path ASSET_PATH("../../assets");
static const std::string SHADER_EXT("-debug.spv");
#else
static const std::filesystem::path ASSET_PATH("assets");
static const std::string SHADER_EXT("-release.spv");
#endif

} // namespace vkl

#endif // VKLEARNIN_VKLEARNIN_HPP