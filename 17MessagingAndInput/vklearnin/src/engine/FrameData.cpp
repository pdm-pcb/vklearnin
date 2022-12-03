#include "vklearnin/vklearnin.hpp"
#include "vklearnin/engine/FrameData.hpp"

#include "vklearnin/rendering/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void FrameData::create() {
    _command_pool.create();
    _command_buffer.create(_command_pool);
}

// =============================================================================
void FrameData::destroy() {
    _command_buffer.destroy();
    _command_pool.destroy();
}

// =============================================================================
FrameData::FrameData(FrameData &&other) :
    _command_pool    { std::move(other._command_pool) },
    _command_buffer  { std::move(other._command_buffer) }
{ }

} // namespace vkl