#include "vklearnin/vklearnin.hpp"
#include "vklearnin/render/renderpass/Framebuffer.hpp"

#include "vklearnin/render/renderpass/RenderPass.hpp"
#include "vklearnin/render/swapchain/Swapchain.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void Framebuffer::create(const std::vector<vk::ImageView> &attachments,
                         const RenderPass &render_pass)
{
    const vk::FramebufferCreateInfo buffer_info {
        .renderPass      = render_pass.native(),
        .attachmentCount = static_cast<uint32_t>(attachments.size()),
        .pAttachments    = attachments.data(),
        .width           = Swapchain::extent().width,
        .height          = Swapchain::extent().height,
        .layers          = 1u,
    };

    auto result = LogicalDevice::native().createFramebuffer(buffer_info);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Failed to create framebuffer: '{}'",
            to_string(result.result)
        );
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