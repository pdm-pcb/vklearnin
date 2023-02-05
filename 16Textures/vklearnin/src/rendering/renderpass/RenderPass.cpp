#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/renderpass/RenderPass.hpp"

#include "vklearnin/system/devices/PhysicalDevice.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"
#include "vklearnin/rendering/swapchain/Swapchain.hpp"

namespace vkl {

// =============================================================================
void RenderPass::create() {
    _find_depth_stencil_format();
    _init_color_buffer();
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
    ImageTools::destroy(_depth_buffer);
    ImageTools::destroy(_color_buffer);
    LogicalDevice::native().destroyRenderPass(_render_pass);
    _render_pass = nullptr;
}

// =============================================================================
void RenderPass::_default_attachments() {
    _attach_descs = {{
        // color buffer (msaa) attachment description
        .format         = Swapchain::image_format(),
        .samples        = _samples,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .storeOp        = vk::AttachmentStoreOp::eDontCare,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::eColorAttachmentOptimal,
    },
    {   // depth buffer attachment description
        .format         = _depth_buffer.format,
        .samples        = _samples,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .storeOp        = vk::AttachmentStoreOp::eDontCare,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
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

        // With whatever MSAA samples we've got
        .pResolveAttachments = _resolve_attachments.data(),

        // With a depth stencil
        .pDepthStencilAttachment = &_depth_attachment,

        // As we've only got a single subpass, there's nothing to preserve
        // between subpasses
        .preserveAttachmentCount = 0u,
        .pPreserveAttachments    = nullptr,
    }};

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
}

// =============================================================================
void RenderPass::_init_color_buffer() {
    if(_color_buffer.handle) {
        ImageTools::destroy(_color_buffer);
    }

    _get_sample_count();

    _color_buffer.format = Swapchain::image_format();
    _color_buffer.extent = {
        .width  = Swapchain::extent().width,
        .height = Swapchain::extent().height,
        .depth  = 1u
    };

    ImageTools::create(
        _color_buffer,
        vk::ImageType::e2D,
        _samples,
        (
            vk::ImageUsageFlagBits::eColorAttachment |
            vk::ImageUsageFlagBits::eTransientAttachment // ??????????????????????????????????????????????????????????????????
        ),
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    ImageTools::create_view(
        _color_buffer,
        vk::ImageViewType::e2D,
        vk::ImageAspectFlagBits::eColor
    );
}

// =============================================================================
void RenderPass::_get_sample_count() {
    switch(RenderConfig::sample_count) {
        case 64u: _samples = vk::SampleCountFlagBits::e64; break;
        case 32u: _samples = vk::SampleCountFlagBits::e32; break;
        case 16u: _samples = vk::SampleCountFlagBits::e16; break;
        case 8u:  _samples = vk::SampleCountFlagBits::e8;  break;
        case 4u:  _samples = vk::SampleCountFlagBits::e4;  break;
        case 2u:  _samples = vk::SampleCountFlagBits::e2;  break;
        case 1u:  _samples = vk::SampleCountFlagBits::e1;  break;
        default:
            CONSOLE_CRITICAL(
                "Unsupported color buffer sample count {}.",
                RenderConfig::sample_count
            );
    }
}

// =============================================================================
void RenderPass::_init_depth_buffer() {
    if(_depth_buffer.handle) {
        ImageTools::destroy(_depth_buffer);
    }

    _depth_buffer.extent = {
        .width  = Swapchain::extent().width,
        .height = Swapchain::extent().height,
        .depth  = 1u
    };

    ImageTools::create(
        _depth_buffer,
        vk::ImageType::e2D,
        _samples,
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

    for(auto const& option : depth_options) {
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
    _attach_descs        { },
    _color_attachments   { },
    _depth_attachment    { },
    _resolve_attachments { }, 
    _subpasses           { },
    _subpass_deps        { },
    _render_pass         { },
    _samples             { },
    _color_buffer        { },
    _depth_buffer        { }
{ }

} // namespace vkl