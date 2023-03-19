#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/renderpass/Framebuffer.hpp"

#include "vklearnin/rendering/renderpass/RenderPass.hpp"
#include "vklearnin/rendering/swapchain/Swapchain.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void Framebuffer::create(const std::vector<vk::ImageView> &attachments,
                         const vk::RenderPass &render_pass)
{
    const vk::FramebufferCreateInfo buffer_info {
        .renderPass      = render_pass,
        .attachmentCount = static_cast<uint32_t>(attachments.size()),
        .pAttachments    = attachments.data(),
        .width           = Swapchain::extent().width,
        .height          = Swapchain::extent().height,
        .layers          = 1u,
    };

    _framebuffer = LogicalDevice::native().createFramebuffer(buffer_info);

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

    LogicalDevice::native().destroyFramebuffer(_framebuffer);

    _framebuffer = vk::Framebuffer();
}

// =============================================================================
Framebuffer::Framebuffer() :
    _framebuffer { }
{ }

Framebuffer::Framebuffer(Framebuffer &&other) noexcept :
    _framebuffer { other._framebuffer }
{
    other._framebuffer = nullptr;
}

} // namespace vkl