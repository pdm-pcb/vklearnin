#ifndef VKLEARNIN_COMMON_HPP
#define VKLEARNIN_COMMON_HPP

// #define MEMLOG
#include "vklearnin/Tools/MemTracker.hpp"
#include "vklearnin/Tools/ConsoleLog.hpp"
#include "vklearnin/Tools/VKDebugger.hpp"
#include "vklearnin/Tools/Allocator.hpp"
#include "vklearnin/Tools/BufferTools.hpp"
#include "vklearnin/Tools/ImageTools.hpp"

#include "vklearnin/System/Events/EventBroker.hpp"
// #include "vklearnin/System/Events/KeyboardEvent.hpp"

#include "vklearnin/Shaders/Shader.hpp"
#include "vklearnin/Shaders/Index.hpp"
#include "vklearnin/Shaders/Vertex.hpp"
#include "vklearnin/Shaders/MVPMatrices.hpp"

static constexpr char APP_NAME[] { "Learnin'" };
static constexpr char ENGINE_NAME[] { "Vulkan Learnin'" };
static constexpr uint32_t APP_VERSION = VK_MAKE_API_VERSION(0, 0, 1, 0);
static constexpr uint32_t ENGINE_VERSION = VK_MAKE_API_VERSION(0, 0, 1, 0);

static constexpr uint32_t VK_API_VER = VK_API_VERSION_1_1;

static constexpr size_t PHYSICAL_DEVICE = 0;

static constexpr uint32_t UI32MAX = std::numeric_limits<uint32_t>::max();
static constexpr uint64_t UI64MAX = std::numeric_limits<uint64_t>::max();
static constexpr uint32_t FRAME_OVERLAP = 2u;
static constexpr uint32_t MSAA_SAMPLES = 8u;

static constexpr float SAMPLE_SHADING_RATE = 1.0f;

static constexpr uint32_t MAX_DESC_SETS        = 1024u;
static constexpr uint32_t MAX_DESC_COUNT       = 1024u;
static constexpr uint32_t CAM_UBO_BINDING      = 0u;
static constexpr uint32_t SAMPLER_BINDING      = 0u;
static constexpr uint32_t MODEL_MATRIX_BINDING = 0u;

#endif // VKLEARNIN_COMMON_HPP