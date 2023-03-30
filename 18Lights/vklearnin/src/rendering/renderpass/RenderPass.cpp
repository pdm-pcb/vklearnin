#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/renderpass/RenderPass.hpp"

#include "vklearnin/system/devices/PhysicalDevice.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"
#include "vklearnin/rendering/swapchain/Swapchain.hpp"

namespace vkl {

// =============================================================================
RenderPass & RenderPass::create_color_buffer(vk::Extent2D const &extent) {
    if(_color_buffer.handle) {
        ImageTools::destroy(_color_buffer);
    }

    _color_buffer.format = Swapchain::image_format();
    _color_buffer.extent = vk::Extent3D {
        .width  = extent.width,
        .height = extent.height,
        .depth  = 1u
    };

    ImageTools::create(
        _color_buffer,
        vk::ImageType::e2D,
        vk::ImageAspectFlagBits::eColor,
        RenderConfig::max_msaa_flag(),
        (
            vk::ImageUsageFlagBits::eColorAttachment |
            vk::ImageUsageFlagBits::eTransientAttachment
        ),
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    ImageTools::create_view(
        _color_buffer,
        vk::ImageViewType::e2D,
        vk::ImageAspectFlagBits::eColor
    );

    return *this;
}

// =============================================================================
RenderPass & RenderPass::create_depth_buffer(vk::Extent2D const &extent) {
    if(_depth_buffer.handle) {
        ImageTools::destroy(_depth_buffer);
    }

    _depth_buffer.format = PhysicalDevice::depth_format();
    _depth_buffer.extent = vk::Extent3D {
        .width  = extent.width,
        .height = extent.height,
        .depth  = 1u
    };

    ImageTools::create(
        _depth_buffer,
        vk::ImageType::e2D,
        vk::ImageAspectFlagBits::eDepth,
        RenderConfig::max_msaa_flag(),
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    ImageTools::create_view(
        _depth_buffer,
        vk::ImageViewType::e2D,
        vk::ImageAspectFlagBits::eDepth
    );

    return *this;
}

// =============================================================================
RenderPass & RenderPass::default_color_attachments() {
    _attach_descs = {{
        // color buffer (msaa) attachment description
        .format         = Swapchain::image_format(),
        .samples        = RenderConfig::max_msaa_flag(),
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .storeOp        = vk::AttachmentStoreOp::eDontCare,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::eColorAttachmentOptimal,
    },
    {   // depth buffer attachment description
        .format         = _depth_buffer.format,
        .samples        = RenderConfig::max_msaa_flag(),
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .storeOp        = vk::AttachmentStoreOp::eDontCare,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    },
    {   // final presentation attachment
        .format         = Swapchain::image_format(),
        .samples        = vk::SampleCountFlagBits::e1,
        .loadOp         = vk::AttachmentLoadOp::eDontCare,
        .storeOp        = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
    }};

    _color_attachments = {{
        .attachment = 0u,
        .layout     = vk::ImageLayout::eColorAttachmentOptimal,
    }};

    _depth_attachment = {
        .attachment = 1u,
        .layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };

    _resolve_attachments = {{
        .attachment = 2u,
        .layout = vk::ImageLayout::eColorAttachmentOptimal,
    }};

    return *this;
}

// =============================================================================
RenderPass & RenderPass::default_color_subpass() {
    _subpass_deps = {{
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0u,
        .srcStageMask    = vk::PipelineStageFlagBits::eBottomOfPipe,
        .dstStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .srcAccessMask   = vk::AccessFlagBits::eMemoryRead,
        .dstAccessMask   = vk::AccessFlagBits::eColorAttachmentWrite,
        .dependencyFlags = vk::DependencyFlagBits::eByRegion
    },
    {
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0u,
        .srcStageMask    = (vk::PipelineStageFlagBits::eEarlyFragmentTests |
                            vk::PipelineStageFlagBits::eLateFragmentTests),
        .dstStageMask    = (vk::PipelineStageFlagBits::eEarlyFragmentTests |
                            vk::PipelineStageFlagBits::eLateFragmentTests),
        .srcAccessMask   = vk::AccessFlagBits::eNone,
        .dstAccessMask   = vk::AccessFlagBits::eDepthStencilAttachmentWrite,
        .dependencyFlags = vk::DependencyFlagBits::eByRegion
    }};

    _subpasses = {{
        // This subpass is a graphical one
        .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,

        // ...Which has no input of any kind
        .inputAttachmentCount = 0u,
        .pInputAttachments    = nullptr,

        // But does have a single color attachment
        .colorAttachmentCount =
            static_cast<uint32_t>(_color_attachments.size()),
        .pColorAttachments = _color_attachments.data(),

        // With whatever MSAA samples we've got
        .pResolveAttachments = _resolve_attachments.data(),

        // With a depth stencil
        .pDepthStencilAttachment = &_depth_attachment,

        // As we've only got a single subpass, there's nothing to preserve
        // between subpasses
        .preserveAttachmentCount = 0u,
        .pPreserveAttachments    = nullptr,
    }};

    return *this;
}

// =============================================================================
RenderPass & RenderPass::create_shadow_map(vk::Extent2D const &extent) {
    _shadow_map.destroy();

    _shadow_map.create_shadow_map(extent, PhysicalDevice::depth_format());
    _shadow_map.create_sampler(
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear,
        vk::SamplerAddressMode::eClampToBorder,
        vk::SamplerAddressMode::eClampToBorder,
        VK_TRUE,
        vk::CompareOp::eLessOrEqual
    );

    return *this;
}

// =============================================================================
RenderPass & RenderPass::default_shadow_map_attachments() {
    _attach_descs = {{
        .format         = PhysicalDevice::depth_format(),
        .samples        = vk::SampleCountFlagBits::e1,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .storeOp        = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::eDepthStencilReadOnlyOptimal,
    }};

    _shadow_map_attachment = {
        .attachment = 0u,
        .layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };

    return *this;
}

// =============================================================================
RenderPass & RenderPass::default_shadow_map_subpass() {
    _subpass_deps = {{
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0u,
        .srcStageMask    = vk::PipelineStageFlagBits::eFragmentShader,
        .dstStageMask    = vk::PipelineStageFlagBits::eEarlyFragmentTests,
        .srcAccessMask   = vk::AccessFlagBits::eShaderRead,
        .dstAccessMask   = vk::AccessFlagBits::eDepthStencilAttachmentWrite,
        .dependencyFlags = vk::DependencyFlagBits::eByRegion
    },
    {
        .srcSubpass      = 0u,
        .dstSubpass      = VK_SUBPASS_EXTERNAL,
        .srcStageMask    = vk::PipelineStageFlagBits::eLateFragmentTests,
        .dstStageMask    = vk::PipelineStageFlagBits::eFragmentShader,
        .srcAccessMask   = vk::AccessFlagBits::eDepthStencilAttachmentWrite,
        .dstAccessMask   = vk::AccessFlagBits::eShaderRead,
        .dependencyFlags = vk::DependencyFlagBits::eByRegion
    }};

    _subpasses = {{
        .pipelineBindPoint       = vk::PipelineBindPoint::eGraphics,
        .pDepthStencilAttachment = &_shadow_map_attachment,
    }};

    return *this;
}

// =============================================================================
void RenderPass::create() {
    const vk::RenderPassCreateInfo renderpass_info {
        .attachmentCount = static_cast<uint32_t>(_attach_descs.size()),
        .pAttachments    = _attach_descs.data(),
        .subpassCount    = static_cast<uint32_t>(_subpasses.size()),
        .pSubpasses      = _subpasses.data(),
        .dependencyCount = static_cast<uint32_t>(_subpass_deps.size()),
        .pDependencies   = _subpass_deps.data()
    };

    _render_pass = LogicalDevice::native().createRenderPass(renderpass_info);

    CONSOLE_TRACE(
        "Created Render Pass {:#x}",
        reinterpret_cast<uint64_t>(VkRenderPass(_render_pass))
    );
}

// =============================================================================
void RenderPass::destroy() {
    CONSOLE_TRACE(
        "Destroying Render Pass {:#x}",
        reinterpret_cast<uint64_t>(VkRenderPass(_render_pass))
    );

    if(_depth_buffer.handle) {
        ImageTools::destroy(_depth_buffer);
    }
    if(_color_buffer.handle) {
        ImageTools::destroy(_color_buffer);
    }

    _shadow_map.destroy();

    LogicalDevice::native().destroyRenderPass(_render_pass);
    _render_pass = nullptr;
}

// =============================================================================
RenderPass::RenderPass() :
    _attach_descs        { },
    _color_attachments   { },
    _depth_attachment    { },
    _resolve_attachments { },
    _subpass_deps        { },
    _subpasses           { },
    _render_pass         { },
    _color_buffer        { },
    _depth_buffer        { },
    _shadow_map          { }
{ }

} // namespace vkl