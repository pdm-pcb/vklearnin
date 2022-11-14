#include "vklearnin/vklearnin.hpp"
#include "vklearnin/engine/Framebuffer.hpp"

#include "vklearnin/rendering/LogicalDevice.hpp"
#include "vklearnin/engine/Swapchain.hpp"
#include "vklearnin/engine/Pipeline.hpp"

namespace vkl {

// =============================================================================
void Framebuffer::create(const Swapchain &swapchain, const Pipeline &pipeline,
                         const uint32_t image_index)
{
    // grab the new dimensions
    auto [width, height] = swapchain.extent();

    _attachments.clear();
    _attachments.emplace_back(swapchain.image_view(image_index));
    
    vk::FramebufferCreateInfo buffer_info {
        .renderPass = pipeline.renderpass(),
        .attachmentCount = static_cast<uint32_t>(_attachments.size()),
        .pAttachments = _attachments.data(),
        .width = width,
        .height = height,
        .layers = 1u,
    };

    auto result = LogicalDevice::native().createFramebuffer(buffer_info);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to create framebuffer");
    }
    _framebuffer = result.value;

    CONSOLE_TRACE(
        "Created framebuffer {:#x}",
        reinterpret_cast<uint64_t>(VkFramebuffer(_framebuffer))
    );
}

// =============================================================================
void Framebuffer::destroy() {
    CONSOLE_TRACE(
        "Destroying framebuffer {:#x}",
        reinterpret_cast<uint64_t>(VkFramebuffer(_framebuffer))
    );
    LogicalDevice::native().destroy(_framebuffer);
}

// =============================================================================
Framebuffer::Framebuffer(Framebuffer &&other) :
    _framebuffer { std::move(other._framebuffer) },
    _attachments { std::move(other._attachments) }
{ }

} // namespace vkl