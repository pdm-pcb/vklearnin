#ifndef VKLEARNIN_VKLEARNIN_HPP
#define VKLEARNIN_VKLEARNIN_HPP

// #define MEMLOG
#include "vklearnin/tools/MemTracker.hpp"
#include "vklearnin/tools/ConsoleLog.hpp"
#include "vklearnin/tools/VKDebugger.hpp"
#include "vklearnin/system/Application.hpp"

#include "vklearnin/rendering/RenderConfig.hpp"
#include "vklearnin/rendering/Renderer.hpp"
#include "vklearnin/engine/Pipeline.hpp"
#include "vklearnin/engine/Swapchain.hpp"
#include "vklearnin/engine/FrameData.hpp"
#include "vklearnin/shaders/DescriptorSet.hpp"

#include "vklearnin/tools/ImageTools.hpp"
#include "vklearnin/tools/BufferTools.hpp"

namespace vkl {

// These constants help Vulkan advertise the application/engine to the drivers,
// in case there are any specific optimizations
static constexpr char APP_NAME[] { "vklearnin: Demo" };
static constexpr char ENGINE_NAME[] { "vklearnin" };
static constexpr uint32_t APP_VERSION    = VK_MAKE_API_VERSION(0, 0, 1, 0);
static constexpr uint32_t ENGINE_VERSION = VK_MAKE_API_VERSION(0, 0, 1, 0);

static constexpr uint32_t VK_API_VER = VK_API_VERSION_1_1;

static constexpr uint32_t DEFAULT_GPU = 0u;

} // namespace vkl
#endif // VKLEARNIN_VKLEARNIN_HPP