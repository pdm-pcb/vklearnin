#include "vklearnin/vklearnin.hpp"
#include "vklearnin/render_passes/ColorDepthResolvePass.hpp"

#include "vklearnin/vulkan/swapchain/vkSurface.hpp"
#include "vklearnin/vulkan/devices/vkPhysicalDevice.hpp"
#include "vklearnin/vulkan/devices/vkDevice.hpp"
#include "vklearnin/vulkan/swapchain/vkFrameBuffer.hpp"
#include "vklearnin/vulkan/devices/vkCmdBuffer.hpp"

namespace vkl {

// =============================================================================
bool ColorDepthResolvePass::create(vkSurface const &surface,
                                   vkPhysicalDevice const &physical_device,
                                   vkDevice const &device,
                                   vk::SampleCountFlagBits const msaa_samples)
{
    if(_render_pass.native()) {
        Log::error(
            "Color depth pass {} already exists.",
            _render_pass.native()
        );
        return false;
    }

    if(!surface.native()) {
        Log::error("Cannot create color depth resolve pass with invalid surface.");
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot create color depth resolve pass with invalid device.");
        return false;
    }

    if(!_find_depth_format(physical_device)) {
        Log::error("Unable to find suitable depth format.");
        return false;
    }

    _color_format = surface.format().format;
    _msaa_samples = msaa_samples;

    _init_attachments();
    _init_subpasses();

    if(!_render_pass.create(
        _attachment_descriptions,
        {{ _subpass_desc }},
        _subpass_deps,
        device
    ))
    {
        Log::error("Failed to create color depth resolve pass.");

        _attachment_descriptions.clear();

        _multisample_ref = vk::AttachmentReference { };
        _depth_ref       = vk::AttachmentReference { };
        _resolve_ref     = vk::AttachmentReference { };

        _subpass_desc = vk::SubpassDescription { };
        _subpass_deps.clear();

        _color_format = vk::Format::eUndefined;
        _depth_format = vk::Format::eUndefined;

        return false;
    }

    _render_area = vk::Rect2D {
        .offset = { },
        .extent = surface.extent()
    };

    _create_multisample_buffer(physical_device, device);

    if(!_create_depth_buffer(physical_device, device)) {

        _attachment_descriptions.clear();

        _multisample_ref = vk::AttachmentReference { };
        _depth_ref       = vk::AttachmentReference { };
        _resolve_ref     = vk::AttachmentReference { };

        _subpass_desc = vk::SubpassDescription { };
        _subpass_deps.clear();

        _render_area = vk::Rect2D { };

        _color_format = vk::Format::eUndefined;
        _depth_format = vk::Format::eUndefined;

        return false;
    }

    return true;
}

// =============================================================================
bool ColorDepthResolvePass::destroy() {
    if(!_render_pass.native()) {
        Log::error("Create color depth resolve pass before calling destroy.");
        return false;
    }

    _render_pass.destroy();

    _attachment_descriptions.clear();

    _multisample_ref = vk::AttachmentReference { };
    _depth_ref       = vk::AttachmentReference { };
    _resolve_ref     = vk::AttachmentReference { };

    _subpass_desc = vk::SubpassDescription { };
    _subpass_deps.clear();

    _render_area = vk::Rect2D { };

    _color_format = vk::Format::eUndefined;
    _depth_format = vk::Format::eUndefined;

    _destroy_multisample_buffer();
    _destroy_depth_buffer();

    return true;
}

// =============================================================================
void ColorDepthResolvePass::destroy_swapchain_resources() {
    _render_area = vk::Rect2D { };

    _color_format = vk::Format::eUndefined;
    _depth_format = vk::Format::eUndefined;

    _destroy_multisample_buffer();
    _destroy_depth_buffer();
}

// =============================================================================
void ColorDepthResolvePass::create_swapchain_resources(
    vkSurface const &surface,
    vkPhysicalDevice const &physical_device,
    vkDevice const &device)
{
    _find_depth_format(physical_device);
    _color_format = surface.format().format;

    _render_area = vk::Rect2D {
        .offset = { },
        .extent = surface.extent()
    };

    _create_multisample_buffer(physical_device, device);
    _create_depth_buffer(physical_device, device);
}

// =============================================================================
void ColorDepthResolvePass::begin(
    vkFrameBuffer const &frame_buffer,
    std::span<vk::ClearValue const> const clear_values,
    vkCmdBuffer const &cmd_buffer)
{
    auto const begin_info = vk::RenderPassBeginInfo {
        .pNext = nullptr,
        .renderPass = _render_pass.native(),
        .framebuffer = frame_buffer.native(),
        .renderArea = _render_area,
        .clearValueCount = static_cast<uint32_t>(clear_values.size()),
        .pClearValues = clear_values.data(),
    };

    cmd_buffer.native().beginRenderPass(begin_info,
                                        vk::SubpassContents::eInline);
}

// =============================================================================
bool ColorDepthResolvePass::_find_depth_format(
    vkPhysicalDevice const &physical_device)
{
    static std::array<vk::Format const, 2> const depth_formats {
        vk::Format::eD32SfloatS8Uint, // One of these two will always be
        vk::Format::eD24UnormS8Uint,  // supported, according to the Guide.
    };

    for(auto const format : depth_formats) {
        auto props = physical_device.native().getFormatProperties(format);
        if(props.optimalTilingFeatures &
           vk::FormatFeatureFlagBits::eDepthStencilAttachment)
        {
            _depth_format = format;
            return true;
        }
    }

    return false;
}

// =============================================================================
void ColorDepthResolvePass::_init_attachments() {
    _attachment_descriptions = {{
        // multisample buffer attachment description
        .format         = _color_format,
        .samples        = _msaa_samples,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .storeOp        = vk::AttachmentStoreOp::eDontCare,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::eColorAttachmentOptimal,
    },
    {   // depth buffer attachment description
        .format         = _depth_format,
        .samples        = _msaa_samples,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .storeOp        = vk::AttachmentStoreOp::eDontCare,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    },
    {   // resolve attachment description
        .format         = _color_format,
        .samples        = vk::SampleCountFlagBits::e1,
        .loadOp         = vk::AttachmentLoadOp::eDontCare,
        .storeOp        = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
    }};

    _multisample_ref = vk::AttachmentReference {
        .attachment = 0u,
        .layout = vk::ImageLayout::eColorAttachmentOptimal,
    };

    _depth_ref = vk::AttachmentReference {
        .attachment = 1u,
        .layout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };

    _resolve_ref = vk::AttachmentReference {
        .attachment = 2u,
        .layout = vk::ImageLayout::eColorAttachmentOptimal,
    };
}

// =============================================================================
void ColorDepthResolvePass::_init_subpasses() {
    _subpass_desc = vk::SubpassDescription {
        // This subpass is a graphical one
        .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,

        // ...Which has no input of any kind
        .inputAttachmentCount = 0u,
        .pInputAttachments    = nullptr,

        // But does have a single color attachment
        .colorAttachmentCount = 1u,
        .pColorAttachments   = &_multisample_ref,

        // With whatever MSAA samples we've got
        .pResolveAttachments = &_resolve_ref,

        // With a depth stencil
        .pDepthStencilAttachment = &_depth_ref,

        // As we've only got a single subpass, there's nothing to preserve
        // between subpasses
        .preserveAttachmentCount = 0u,
        .pPreserveAttachments    = nullptr,
    };

    _subpass_deps = {
        vk::SubpassDependency {
            .srcSubpass = vk::SubpassExternal,
            .dstSubpass = 0u,

            .srcStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests
                            | vk::PipelineStageFlagBits::eLateFragmentTests,

            .dstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests
                            | vk::PipelineStageFlagBits::eLateFragmentTests,

            .srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite,

            .dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite,

            .dependencyFlags = { },
        },
        vk::SubpassDependency {
            .srcSubpass = vk::SubpassExternal,
            .dstSubpass = 0u,

            .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,

            .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,

            .srcAccessMask = { },

            .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite
                             | vk::AccessFlagBits::eColorAttachmentRead,

            .dependencyFlags = { },
        },
    };
}

// =============================================================================
bool ColorDepthResolvePass::_create_multisample_buffer(
    vkPhysicalDevice const &physical_device,
    vkDevice const &device)
{
    vkImage::Details const details {
        .type         = vk::ImageType::e2D,
        .samples      = _msaa_samples,
        .usage_flags  = (vk::ImageUsageFlagBits::eColorAttachment |
                         vk::ImageUsageFlagBits::eTransientAttachment),
        .memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
    };

    Log::trace("Creating multisample buffer for color depth resolve pass");

    _multisample_buffer.create(
        _render_area.extent,
        _color_format,
        details,
        physical_device,
        device
    );

    _multisample_view.create(
        vkImageView::Details {
            .image        = _multisample_buffer.native(),
            .format       = _multisample_buffer.format(),
            .type         = vk::ImageViewType::e2D,
            .aspect_flags = vk::ImageAspectFlagBits::eColor
        },
        device
    );

    return true;
}

// =============================================================================
void ColorDepthResolvePass::_destroy_multisample_buffer() {
    _multisample_view.destroy();
    _multisample_buffer.destroy();
}

// =============================================================================
bool ColorDepthResolvePass::_create_depth_buffer(
    vkPhysicalDevice const &physical_device,
    vkDevice const &device)
{
    vkImage::Details const details {
        .type         = vk::ImageType::e2D,
        .samples      = _msaa_samples,
        .usage_flags  = vk::ImageUsageFlagBits::eDepthStencilAttachment,
        .memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
    };

    Log::trace("Creating depth buffer for color depth resolve pass");

    if(!_depth_buffer.create(
        _render_area.extent,
        _depth_format,
        details,
        physical_device,
        device
    ))
    {
        Log::error("Failed to create depth buffer.");
        return false;
    }

    if(!_depth_view.create(
        vkImageView::Details {
            .image        = _depth_buffer.native(),
            .format       = _depth_buffer.format(),
            .type         = vk::ImageViewType::e2D,
            .aspect_flags = vk::ImageAspectFlagBits::eDepth,
        },
        device
    ))
    {
        Log::error("Failed to create depth view.");
        _depth_buffer.destroy();
        return false;
    }

    return true;
}

// =============================================================================
void ColorDepthResolvePass::_destroy_depth_buffer() {
    _depth_view.destroy();
    _depth_buffer.destroy();
}

} // namespace vkl