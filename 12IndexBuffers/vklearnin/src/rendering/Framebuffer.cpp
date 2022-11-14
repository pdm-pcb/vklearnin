#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/Framebuffer.hpp"

#include "vklearnin/rendering/Swapchain.hpp"
#include "vklearnin/rendering/Pipeline.hpp"

namespace vkl {

// =============================================================================
void Framebuffer::create(uint32_t buffer_idx) {
    // grab the new dimensions
    auto [width, height] = _swapchain.extent();

    _attachments.clear();
    _attachments.emplace_back(_swapchain.image_views()[buffer_idx]);
    
    vk::FramebufferCreateInfo buffer_info {
        .renderPass = _pipeline.renderpass(),
        .attachmentCount = static_cast<uint32_t>(_attachments.size()),
        .pAttachments = _attachments.data(),
        .width = width,
        .height = height,
        .layers = 1u,
    };

    auto result = _logical_device.createFramebuffer(buffer_info);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to create framebuffer");
    }
    _framebuffer = result.value;

    CONSOLE_TRACE(
        "Created framebuffer {}",
        reinterpret_cast<uint64_t>(VkFramebuffer(_framebuffer))
    );
}

// =============================================================================
void Framebuffer::destroy() {
    _logical_device.destroy(_framebuffer);
}

// =============================================================================
Framebuffer::Framebuffer(const vk::Device &logical_device,
                         const Swapchain &swapchain,
                         const Pipeline &pipeline) :
    _logical_device { logical_device },
    _swapchain      { swapchain },
    _pipeline       { pipeline }
{ }

Framebuffer::Framebuffer(Framebuffer &&other) :
    _framebuffer    { std::move(other._framebuffer) },
    _attachments    { std::move(other._attachments) },
    _logical_device { std::move(other._logical_device) },
    _swapchain      { std::move(other._swapchain) },
    _pipeline       { std::move(other._pipeline) }
{ }

} // namespace vkl