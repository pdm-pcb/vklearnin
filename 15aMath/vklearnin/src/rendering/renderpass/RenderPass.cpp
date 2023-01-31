#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/renderpass/RenderPass.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"
#include "vklearnin/rendering/swapchain/Swapchain.hpp"

namespace vkl {

// =============================================================================
void RenderPass::create() {
    _default_attachments();
    _default_subpasses();

    const vk::RenderPassCreateInfo renderpass_info {
        .attachmentCount = static_cast<uint32_t>(_attach_descs.size()),
        .pAttachments    = _attach_descs.data(),
        .subpassCount    = static_cast<uint32_t>(_subpasses.size()),
        .pSubpasses      = _subpasses.data(),
        .dependencyCount = static_cast<uint32_t>(_subpass_deps.size()),
        .pDependencies   = _subpass_deps.data()
    };

    auto result = LogicalDevice::native().createRenderPass(renderpass_info);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Failed to create render pass: '{}'",
            to_string(result.result)
        );
    }
    _render_pass = result.value;

    CONSOLE_TRACE(
        "Created Render Pass {:#x}",
        reinterpret_cast<uint64_t>(VkRenderPass(_render_pass))
    );
}

// =============================================================================
void RenderPass::destroy() {
    LogicalDevice::native().destroyRenderPass(_render_pass);
    _render_pass = nullptr;
}

// =============================================================================
void RenderPass::_default_attachments() {
    _attach_descs = {{
        // The image format for the color attachment must match that of the
        // swapchain we're using to present
        .format = Swapchain::image_format(),
        
        // No multisampling... yet
        .samples = vk::SampleCountFlagBits::e1,
        
        // Clear any residual information so we're literally working with a
        // blank canvas
        .loadOp = vk::AttachmentLoadOp::eClear,
        
        // Notify Vulkan that we'd like  to keep whatever we've drawn
        .storeOp = vk::AttachmentStoreOp::eStore,
        
        // Stencil operations aren't useful yet
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        
        // We don't care what layout the image data has when we receive it
        // initially, but once we're done, the image is ready to be presented
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
    }};

    _color_attachments = {{
        .attachment = 0u,
        .layout     = vk::ImageLayout::eColorAttachmentOptimal,
    }};
}

// =============================================================================
void RenderPass::_default_subpasses() {
    _subpasses = {{
        // This subpass is a graphical one
        .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,

        // ...Which has no input of any kind
        .inputAttachmentCount = 0u,
        .pInputAttachments    = nullptr,

        // But does have a single color attachment
        .colorAttachmentCount =
            static_cast<uint32_t>(_color_attachments.size()),
        .pColorAttachments   = _color_attachments.data(),

        // And has no multisampling resolution attachments
        .pResolveAttachments = nullptr,

        // Nor any depth attachments
        .pDepthStencilAttachment = nullptr,

        // As we've only got a single subpass, there's nothing to preserve
        // between subpasses
        .preserveAttachmentCount = 0u,
        .pPreserveAttachments    = nullptr,
    }};

    _subpass_deps = {{
        // Marking a dependency external indicates it's outside the scope of
        // this render pass. If the source subpass is external, the dependency
        // is everything before this pass. If the destination subpass is
        // external, the dependency is everything after this pass.
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0u,

        // This subpass will take place after all pipeline stages are complete
        .srcStageMask    = vk::PipelineStageFlagBits::eBottomOfPipe,

        // This subpass will output to the color attachment
        .dstStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput,

        // We want to read from the bottom of the pipe
        .srcAccessMask   = vk::AccessFlagBits::eMemoryRead,

        // We want to write to the color attachment
        .dstAccessMask   = vk::AccessFlagBits::eColorAttachmentWrite,

        // Flagging a subpass dependency by-region means it is local to its
        // framebuffer
        .dependencyFlags = vk::DependencyFlagBits::eByRegion
    }};
}

// =============================================================================
RenderPass::RenderPass() :
    _render_pass { }
{ }

} // namespace vkl