#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/Framebuffer.hpp"

#include "vklearnin/rendering/devices/LogicalDevice.hpp"
#include "vklearnin/engine/Swapchain.hpp"

namespace vkl {

// =============================================================================
void Framebuffer::create(const vk::Extent2D &extent,
                         const std::vector<vk::ImageView> &attachments,
                         const vk::RenderPass &render_pass)
{

    vk::FramebufferCreateInfo buffer_info {
        .renderPass = render_pass,
        .attachmentCount = static_cast<uint32_t>(attachments.size()),
        .pAttachments = attachments.data(),
        .width = extent.width,
        .height = extent.height,
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
    _framebuffer { std::move(other._framebuffer) }
{ }

} // namespace vkl