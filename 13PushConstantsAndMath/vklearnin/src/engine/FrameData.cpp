#include "vklearnin/vklearnin.hpp"
#include "vklearnin/engine/FrameData.hpp"

namespace vkl {

void FrameData::create(const Swapchain &swapchain, const Pipeline &pipeline,
                       const uint32_t image_index)
{
    _framebuffer.create(swapchain, pipeline, image_index);
    _cmd_pool.create();
    _cmd_buffer.create(_cmd_pool);
}

void FrameData::destroy() {
    _framebuffer.destroy();
    _cmd_buffer.destroy();
    _cmd_pool.destroy();
}

FrameData::FrameData(FrameData &&other) :
    _framebuffer { std::move(other._framebuffer) },
    _cmd_pool    { std::move(other._cmd_pool)    },
    _cmd_buffer  { std::move(other._cmd_buffer)  }
{ }

} // namespace vkl