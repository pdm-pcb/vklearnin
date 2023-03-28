#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/renderpass/Framebuffer.hpp"

#include "vklearnin/rendering/renderpass/RenderPass.hpp"
#include "vklearnin/rendering/swapchain/Swapchain.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void Framebuffer::create(vk::Rect2D const &render_area,
                         std::vector<vk::ImageView> const &attachments,
                         vk::RenderPass const &render_pass)
{
    _render_area = render_area;

    const vk::FramebufferCreateInfo buffer_info {
        .renderPass      = render_pass,
        .attachmentCount = static_cast<uint32_t>(attachments.size()),
        .pAttachments    = attachments.data(),
        .width           = _render_area.extent.width,
        .height          = _render_area.extent.height,
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