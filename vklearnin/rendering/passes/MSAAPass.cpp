#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/passes/MSAAPass.hpp"

#include "vklearnin/vulkan/swapchain/vkSurface.hpp"
#include "vklearnin/vulkan/devices/vkPhysicalDevice.hpp"
#include "vklearnin/vulkan/devices/vkDevice.hpp"
#include "vklearnin/vulkan/swapchain/vkFrameBuffer.hpp"
#include "vklearnin/vulkan/devices/vkCmdBuffer.hpp"

namespace vkl {

// =============================================================================
bool MSAAPass::create(vkSurface const &surface,
                      vk::Format const depth_format,
                      std::span<vk::ClearValue const> const clear_values,
                      vk::SampleCountFlagBits const msaa_sample_count,
                      vkPhysicalDevice const &physical_device,
                      vkDevice const &device)
{
    if(_render_pass.native()) {
        Log::error("MSAA pass {} already exists.", _render_pass.native());
        return false;
    }

    if(!surface.native()) {
        Log::error("Cannot create MSAA pass with invalid surface.");
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot create MSAA pass with invalid device.");
        return false;
    }

    _multisample_format = surface.format().format;
    _depth_format       = depth_format;
    _msaa_sample_count  = msaa_sample_count;

    _init_attachments();
    _init_subpasses();

    if(!_render_pass.create(_attachment_descriptions,
                            _subpass_descriptions,
                            _subpass_deps,
                            device))
    {
        Log::error("Failed to create MSAA pass.");
        _reset_object();
        return false;
    }

    if(!_create_multisample_buffer(surface, physical_device, device)) {
        Log::error("Failed to create MSAA multisample buffer.");
        _reset_object();
        return false;
    }

    if(!_create_depth_buffer(surface, physical_device, device)) {
        Log::error("Failed to create MSAA multisample view.");
        _reset_object();
        return false;
    }

    _begin_info = vk::RenderPassBeginInfo {
        .pNext = nullptr,
        .renderPass = _render_pass.native(),
        .framebuffer = { },
        .renderArea = vk::Rect2D {
            .offset = { },
            .extent = surface.extent()
        },
        .clearValueCount = static_cast<uint32_t>(clear_values.size()),
        .pClearValues = clear_values.data(),
    };

    return true;
}

// =============================================================================
bool MSAAPass::destroy() {
    if(!_render_pass.native()) {
        Log::error("Create MSAA pass before calling destroy.");
        return false;
    }

    _reset_object();

    return true;
}

// =============================================================================
void MSAAPass::update_render_area(vkSurface const &surface) {
    _begin_info.renderArea = vk::Rect2D {
        .offset = vk::Offset2D { },
        .extent = surface.extent(),
    };
}

// =============================================================================
void MSAAPass::destroy_swapchain_resources() {
    _begin_info.renderArea = vk::Rect2D { };

    _multisample_format = vk::Format::eUndefined;
    _depth_format       = vk::Format::eUndefined;

    _destroy_multisample_buffer();
    _destroy_depth_buffer();
}

// =============================================================================
void MSAAPass::create_swapchain_resources(
    vkSurface const &surface,
    vk::Format const depth_format,
    vk::SampleCountFlagBits const msaa_sample_count,
    vkPhysicalDevice const &physical_device,
    vkDevice const &device)
{
    _multisample_format = surface.format().format;
    _msaa_sample_count  = msaa_sample_count;
    _depth_format       = depth_format;

    update_render_area(surface);

    _init_attachments();
    _init_subpasses();

    _create_multisample_buffer(surface, physical_device, device);
    _create_depth_buffer(surface, physical_device, device);
}

// =============================================================================
void MSAAPass::_init_attachments() {
    _attachment_descriptions = {{
        // multisample buffer attachment description
        .format         = _multisample_format,
        .samples        = _msaa_sample_count,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .storeOp        = vk::AttachmentStoreOp::eDontCare,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::eColorAttachmentOptimal,
    },
    {   // depth buffer attachment description
        .format         = _depth_format,
        .samples        = _msaa_sample_count,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .storeOp        = vk::AttachmentStoreOp::eDontCare,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    },
    {   // resolve attachment description
        .format         = _multisample_format,
        .samples        = vk::SampleCountFlagBits::e1,
        .loadOp         = vk::AttachmentLoadOp::eDontCare,
        .storeOp        = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
    }};

    _multisample_refs = {{ vk::AttachmentReference {
        .attachment = 0u,
        .layout = vk::ImageLayout::eColorAttachmentOptimal,
    }}};

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
void MSAAPass::_init_subpasses() {
    _subpass_descriptions = {{ vk::SubpassDescription {
        // This subpass is a graphical one
        .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,

        // ...Which has no input of any kind
        .inputAttachmentCount = 0u,
        .pInputAttachments    = nullptr,

        // But does have a single color attachment
        .colorAttachmentCount = static_cast<uint32_t>(_multisample_refs.size()),
        .pColorAttachments    = _multisample_refs.data(),

        // With whatever MSAA samples we've got
        .pResolveAttachments = &_resolve_ref,

        // With a depth stencil
        .pDepthStencilAttachment = &_depth_ref,

        // As we've only got a single subpass, there's nothing to preserve
        // between subpasses
        .preserveAttachmentCount = 0u,
        .pPreserveAttachments    = nullptr,
    }}};

    _subpass_deps = {
        vk::SubpassDependency {
            .srcSubpass = vk::SubpassExternal,
            .dstSubpass = 0u,

            .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput
                            | vk::PipelineStageFlagBits::eLateFragmentTests,

            .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput
                            | vk::PipelineStageFlagBits::eEarlyFragmentTests,

            .srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite
                             | vk::AccessFlagBits::eDepthStencilAttachmentWrite,

            .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite
                             | vk::AccessFlagBits::eDepthStencilAttachmentWrite,

            .dependencyFlags = { },
        },
    };
}

// =============================================================================
bool MSAAPass::_create_multisample_buffer(
    vkSurface const &surface,
    vkPhysicalDevice const &physical_device,
    vkDevice const &device)
{
    vkImage::Details const details {
        .type         = vk::ImageType::e2D,
        .samples      = _msaa_sample_count,
        .usage_flags  = (vk::ImageUsageFlagBits::eColorAttachment |
                         vk::ImageUsageFlagBits::eTransientAttachment),
        .memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
    };

    if(!_multisample_buffer.create(surface.extent(),
                                   _multisample_format,
                                   details,
                                   physical_device,
                                   device))
    {
        Log::error("Failed to create multisample buffer.");
        return false;
    }

    if(!_multisample_view.create(
        vkImageView::Details {
            .image        = _multisample_buffer.native(),
            .format       = _multisample_buffer.format(),
            .type         = vk::ImageViewType::e2D,
            .aspect_flags = vk::ImageAspectFlagBits::eColor
        },
        device
    ))
    {
        Log::error("Failed to create multisample buffer view.");
        _multisample_buffer.destroy();
        return false;
    }

    return true;
}

// =============================================================================
bool MSAAPass::_create_depth_buffer(
    vkSurface const &surface,
    vkPhysicalDevice const &physical_device,
    vkDevice const &device)
{
    vkImage::Details const details {
        .type         = vk::ImageType::e2D,
        .samples      = _msaa_sample_count,
        .usage_flags  = vk::ImageUsageFlagBits::eDepthStencilAttachment,
        .memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
    };

    Log::trace("Creating depth buffer for MSAA pass");

    if(!_depth_buffer.create(surface.extent(),
                             _depth_format,
                             details,
                             physical_device,
                             device))
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
void MSAAPass::_destroy_multisample_buffer() {
    if(_multisample_view.native()) {
        _multisample_view.destroy();
    }

    if(_multisample_buffer.native()) {
        _multisample_buffer.destroy();
    }
}

// =============================================================================
void MSAAPass::_destroy_depth_buffer() {
    if(_depth_view.native()) {
        _depth_view.destroy();
    }

    if(_depth_buffer.native()) {
        _depth_buffer.destroy();
    }
}

// =============================================================================
void MSAAPass::_reset_object() {
    if(_render_pass.native()) {
        _render_pass.destroy();
    }

    _begin_info = vk::RenderPassBeginInfo { };
    _attachment_descriptions.clear();
    _multisample_refs.clear();
    _depth_ref = vk::AttachmentReference { };
    _resolve_ref = vk::AttachmentReference { };

    _subpass_descriptions.clear();
    _subpass_deps.clear();

    _multisample_format = vk::Format::eUndefined;
    _depth_format       = vk::Format::eUndefined;
    _msaa_sample_count  = vk::SampleCountFlagBits { };

    _destroy_depth_buffer();
    _destroy_multisample_buffer();
}

} // namespace vkl