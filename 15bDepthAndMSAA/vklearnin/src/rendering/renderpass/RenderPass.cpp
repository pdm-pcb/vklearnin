#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/renderpass/RenderPass.hpp"

#include "vklearnin/system/devices/PhysicalDevice.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"
#include "vklearnin/rendering/swapchain/Swapchain.hpp"

namespace vkl {

// =============================================================================
void RenderPass::create() {
    _find_depth_stencil_format();
    _init_depth_buffer();
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
    ImageTools::destroy_view(_depth_buffer);
    ImageTools::destroy_image(_depth_buffer);
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
        
        // Stencil operations aren't useful for the color attachment
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        
        // We don't care what layout the image data has when we receive it
        // initially, but once we're done, the image is ready to be presented
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
    },
    {
        .format  = _depth_buffer.format,
        .samples = vk::SampleCountFlagBits::e1,
        .loadOp  = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eDontCare,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    }};

    _color_attachments = {{
        .attachment = 0u,
        .layout     = vk::ImageLayout::eColorAttachmentOptimal,
    }};

    _depth_attachment = {
        .attachment = 1u,
        .layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };
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

        // With a depth stencil
        .pDepthStencilAttachment = &_depth_attachment,

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
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0u,

        .srcStageMask = (
            vk::PipelineStageFlagBits::eColorAttachmentOutput |
            vk::PipelineStageFlagBits::eEarlyFragmentTests
        ),

        .dstStageMask = (
            vk::PipelineStageFlagBits::eColorAttachmentOutput |
            vk::PipelineStageFlagBits::eEarlyFragmentTests
        ),

        .srcAccessMask = vk::AccessFlagBits::eMemoryRead,
    
        .dstAccessMask = (
            vk::AccessFlagBits::eColorAttachmentWrite |
            vk::AccessFlagBits::eDepthStencilAttachmentWrite
        ),

        .dependencyFlags = vk::DependencyFlagBits::eByRegion
    }};
}

// =============================================================================
void RenderPass::_init_depth_buffer() {
    if(_depth_buffer.handle) {
        ImageTools::destroy_image(_depth_buffer);
    }

    ImageTools::create_image(
        _depth_buffer,
        vk::ImageType::e2D,
        { Swapchain::extent().width, Swapchain::extent().height, 1u },
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    ImageTools::create_view(
        _depth_buffer,
        vk::ImageViewType::e2D,
        vk::ImageAspectFlagBits::eDepth
    );
}

// =============================================================================
void RenderPass::_find_depth_stencil_format() {
    const std::vector<vk::Format> depth_options {
        vk::Format::eD32Sfloat,
        vk::Format::eD32SfloatS8Uint,
        vk::Format::eD24UnormS8Uint
    };

    for(const auto &option : depth_options) {
        auto props = PhysicalDevice::native().getFormatProperties(option);
        if(props.optimalTilingFeatures &
           vk::FormatFeatureFlagBits::eDepthStencilAttachment)
        {
            CONSOLE_TRACE(
                "Using depth stencil format {}",
                to_string(option)
            );
            _depth_buffer.format = option;
            return;
        }
    }

    CONSOLE_CRITICAL("Unable to find suitable depth stencil format");
}

// =============================================================================
RenderPass::RenderPass() :
    _attach_descs      { },
    _color_attachments { },
    _depth_attachment  { },
    _subpasses         { },
    _subpass_deps      { },
    _render_pass       { },
    _depth_buffer      { }
{ }

} // namespace vkl