#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/RenderPass.hpp"

#include "vklearnin/engine/Swapchain.hpp"
#include "vklearnin/rendering/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void RenderPass::create_framebuffers(const Swapchain &swapchain) {
    for(uint32_t image_index = 0;
        image_index < RenderConfig::swapchain_image_count;
        ++image_index)
    {
        _framebuffers[image_index].create(
            swapchain.extent(),
            swapchain.image_view(image_index),
            this->native()
        );
    }
}

// =============================================================================
void RenderPass::destroy_framebuffers() {
    for(auto &framebuffer : _framebuffers) {
        framebuffer.destroy();
    }
}

//==============================================================================
void RenderPass::create(const Swapchain &swapchain) {
    _default_attachments(swapchain);
    _default_subpasses();
    _default_subpass_dependencies();

    vk::RenderPassCreateInfo renderpass_info {
        .attachmentCount = static_cast<uint32_t>(_attachments.size()),
        .pAttachments    = _attachments.data(),
        .subpassCount    = static_cast<uint32_t>(_subpasses.size()),
        .pSubpasses      = _subpasses.data(),
        .dependencyCount = static_cast<uint32_t>(_subpass_dependencies.size()),
        .pDependencies   = _subpass_dependencies.data()
    };

    CONSOLE_TRACE(
        "Creating render pass with {} {} and {} {}",
        renderpass_info.attachmentCount,
        renderpass_info.attachmentCount == 1 ? "attachment" : "attachments",
        renderpass_info.subpassCount,
        renderpass_info.subpassCount == 1 ? "subpass" : "subpasses"
    );

    auto result = LogicalDevice::native().createRenderPass(renderpass_info);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to create render pass");
    }
    _render_pass = result.value;

    create_framebuffers(swapchain);
}

//==============================================================================
void RenderPass::destroy() {
    destroy_framebuffers();
    LogicalDevice::native().destroy(_render_pass);
}

//==============================================================================
void RenderPass::_default_attachments(const Swapchain &swapchain) {
    _attachments.clear();
    _attachments = {{
        .format         = swapchain.surface_format(),
        .samples        = vk::SampleCountFlagBits::e1,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .storeOp        = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
    }};
}

//==============================================================================
void RenderPass::_default_subpasses() {
    _color_attachments.clear();
    _color_attachments = {{
        .attachment = 0u,
        .layout     = vk::ImageLayout::eColorAttachmentOptimal,
    }};

    _subpasses.clear();
    _subpasses = {{
        .pipelineBindPoint    = vk::PipelineBindPoint::eGraphics,
        .inputAttachmentCount = 0u,
        .pInputAttachments    = nullptr,
        .colorAttachmentCount =
            static_cast<uint32_t>(_color_attachments.size()),
        .pColorAttachments    = _color_attachments.data(),
        .pResolveAttachments  = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0u,
        .pPreserveAttachments    = 0u,
    }};
}

//==============================================================================
void RenderPass::_default_subpass_dependencies() {
    _subpass_dependencies.clear();
    _subpass_dependencies = {
    {
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0u,
        .srcStageMask    = vk::PipelineStageFlagBits::eBottomOfPipe,
        .dstStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .srcAccessMask   = vk::AccessFlagBits::eMemoryRead,
        .dstAccessMask   = vk::AccessFlagBits::eColorAttachmentWrite,
        .dependencyFlags = vk::DependencyFlagBits::eByRegion
    },
    {
        .srcSubpass      = 0u,
        .dstSubpass      = VK_SUBPASS_EXTERNAL,
        .srcStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .dstStageMask    = vk::PipelineStageFlagBits::eBottomOfPipe,
        .srcAccessMask   = vk::AccessFlagBits::eColorAttachmentWrite,
        .dstAccessMask   = vk::AccessFlagBits::eMemoryRead,
        .dependencyFlags = vk::DependencyFlagBits::eByRegion
    }};
}

//==============================================================================
RenderPass::RenderPass() :
    _render_pass { nullptr }
{
    _framebuffers.resize(RenderConfig::swapchain_image_count);
}

} // namespace vkl