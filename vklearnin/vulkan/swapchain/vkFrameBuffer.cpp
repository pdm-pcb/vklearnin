#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/swapchain/vkFrameBuffer.hpp"

#include "vklearnin/vulkan/devices/vkDevice.hpp"
#include "vklearnin/vulkan/pipelines/vkRenderPass.hpp"

namespace vkl {

// =============================================================================
vkFrameBuffer::vkFrameBuffer(vkFrameBuffer &&other) :
    _handle { other._handle },
    _device { other._device }
{
    other._handle = nullptr;
    other._device = nullptr;
}

// =============================================================================
bool vkFrameBuffer::create(vkRenderPass const &render_pass,
                           std::span<vk::ImageView const> const attachments,
                           vk::Extent2D const &extent,
                           vkDevice const &device)
{
    if(_handle) {
        Log::error("Frame Buffer {} already exists", _handle);
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot create frame buffer with invalid device.");
        return false;
    }

    _device = device.native();

    vk::FramebufferCreateInfo const create_info {
        .pNext = nullptr,
        .flags = { },
        .renderPass = render_pass.native(),
        .attachmentCount = static_cast<uint32_t>(attachments.size()),
        .pAttachments = attachments.data(),
        .width = extent.width,
        .height = extent.height,
        .layers = 1u,
    };

    _handle = _device.createFramebuffer(create_info);
    Log::trace("Created frame buffer {}", _handle);

    return true;
}

// =============================================================================
bool vkFrameBuffer::destroy() {
    if(!_handle) {
        Log::error("Must create frame buffer before calling destroy.");
        return false;
    }

    Log::trace("Destroying frame buffer {}", _handle);
    _device.destroy(_handle);
    _handle = nullptr;
    _device = nullptr;

    return true;
}

} // namespace vkl