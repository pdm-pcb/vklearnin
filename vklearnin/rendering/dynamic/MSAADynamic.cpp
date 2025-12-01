#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/dynamic/MSAADynamic.hpp"

#include "vklearnin/vulkan/swapchain/vkSurface.hpp"
#include "vklearnin/vulkan/devices/vkDevice.hpp"

namespace vkl {

// =============================================================================
void MSAADynamic::init(vkSurface const &surface,
                       std::span<vk::ClearValue const> const clear_values,
                       vk::Format const depth_format,
                       vk::SampleCountFlagBits const msaa_sample_count,
                       vkDevice const &device)
{
    if(!surface.native()) {
        Log::error("Cannot create MSAA dynamic with invalid surface.");
        return;
    }

    if(depth_format == vk::Format::eUndefined) {
        Log::error("Cannot create MSAA dynamic with undefined depth format.");
        return;
    }

    _color_attachment_formats = { surface.format().format };
    _depth_attachment_format  = depth_format;

    _msaa_sample_count = msaa_sample_count;

    if(!_create_depth_buffer(surface, device)) {
        Log::error("Failed to create MSAA dynamic depth buffer.");
        _reset_object();
        return;
    }

    if(!_create_multisample_buffer(surface, device)) {
        Log::error("Failed to create MSAA dynamic multisample buffer.");
        _reset_object();
        return;
    }

    _init_attachments(clear_values);
    _init_rendering_info(surface);
}

// =============================================================================
void MSAADynamic::shutdown() {
    _reset_object();
}

// =============================================================================
vk::RenderingInfo const &
MSAADynamic::rendering_info(vk::ImageView const &view,
                            vk::ImageLayout const &layout)
{
    _color_attachments[0].imageLayout = _multisample_buffer.layout();
    _color_attachments[0].resolveImageView = view;
    _color_attachments[0].resolveImageLayout = layout;

    _depth_attachment.imageLayout = _depth_buffer.layout();

    return _rendering_info;
}

// =============================================================================
void MSAADynamic::update_render_area(vkSurface const &surface) {
    _rendering_info.renderArea = vk::Rect2D {
        .offset = vk::Offset2D { },
        .extent = surface.extent(),
    };
}

// =============================================================================
void MSAADynamic::destroy_swapchain_resources() {
    _rendering_info.renderArea = vk::Rect2D { };

    _color_attachment_formats.clear();
    _depth_attachment_format = vk::Format::eUndefined;

    _destroy_depth_buffer();
    _destroy_multisample_buffer();
}

// =============================================================================
void MSAADynamic::create_swapchain_resources(
    vkSurface const &surface,
    std::span<vk::ClearValue const> const clear_values,
    vk::Format const depth_format,
    vkDevice const &device)
{
    _color_attachment_formats = { surface.format().format };
    _depth_attachment_format    = depth_format;

    update_render_area(surface);
    _create_depth_buffer(surface, device);
    _create_multisample_buffer(surface, device);

    _init_attachments(clear_values);
    _init_rendering_info(surface);
}

// =============================================================================
void MSAADynamic::_init_attachments(
    std::span<vk::ClearValue const> const clear_values)
{
    _color_attachments = {{ vk::RenderingAttachmentInfo {
        .pNext = nullptr,
        .imageView = _multisample_view.native(),
        .imageLayout = { },
        .resolveMode = vk::ResolveModeFlagBits::eAverage,
        .resolveImageView = { },
        .resolveImageLayout = { },
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = {
            .color = clear_values[0].color,
        },
    }}};

    _depth_attachment = vk::RenderingAttachmentInfo {
        .pNext = nullptr,
        .imageView = _depth_view.native(),
        .imageLayout = _depth_buffer.layout(),
        .resolveMode = { },
        .resolveImageView = { },
        .resolveImageLayout = { },
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = {
            .depthStencil = clear_values[1].depthStencil,
        },
    };
}

// =============================================================================
void MSAADynamic::_init_rendering_info(vkSurface const &surface) {
    _rendering_info = vk::RenderingInfo {
        .pNext = nullptr,
        .flags = { },
        .renderArea = vk::Rect2D {
            .offset = { },
            .extent = surface.extent(),
        },
        .layerCount = 1u,
        .viewMask = { },
        .colorAttachmentCount =
            static_cast<uint32_t>(_color_attachments.size()),
        .pColorAttachments = _color_attachments.data(),
        .pDepthAttachment = &_depth_attachment,
        .pStencilAttachment = nullptr,
    };
}

// =============================================================================
bool
MSAADynamic::_create_depth_buffer(vkSurface const &surface,
                                  vkDevice const &device)
{
    vkImage::Details const details {
        .type         = vk::ImageType::e2D,
        .samples      = _msaa_sample_count,
        .usage_flags  = vk::ImageUsageFlagBits::eDepthStencilAttachment,
        .memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
    };

    if(!_depth_buffer.create(surface.extent(),
                             _depth_attachment_format,
                             details,
                             device))
    {
        Log::error("Failed to create MSAA dynamic depth buffer.");
        return false;
    }

    if(!_depth_view.create(
        vkImageView::Details {
            .image        = _depth_buffer.native(),
            .format       = _depth_buffer.format(),
            .type         = vk::ImageViewType::e2D,
            .aspect_flags = vk::ImageAspectFlagBits::eDepth,
        },
        device))
    {
        Log::error("Failed to create MSAA dynamic depth view.");
        return false;
    }

    return true;
}

// =============================================================================
void MSAADynamic::_destroy_depth_buffer() {
    if(_depth_view.native()) {
        _depth_view.destroy();
    }

    if(_depth_buffer.native()) {
        _depth_buffer.destroy();
    }
}

// =============================================================================
bool MSAADynamic::_create_multisample_buffer(vkSurface const &surface,
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
                                   _color_attachment_formats[0],
                                   details,
                                   device))
    {
        Log::error("Failed to create MSAA dynamic multisample buffer.");
        return false;
    }

    if(!_multisample_view.create(
        vkImageView::Details {
            .image        = _multisample_buffer.native(),
            .format       = _multisample_buffer.format(),
            .type         = vk::ImageViewType::e2D,
            .aspect_flags = vk::ImageAspectFlagBits::eColor,
        },
        device))
    {
        Log::error("Failed to create MSAA dynamic multisample view.");
        return false;
    }

    return true;
}

// =============================================================================
void MSAADynamic::_destroy_multisample_buffer() {
    if(_multisample_view.native()) {
        _multisample_view.destroy();
    }

    if(_multisample_buffer.native()) {
        _multisample_buffer.destroy();
    }
}

// =============================================================================
void MSAADynamic::_reset_object() {
    _msaa_sample_count = vk::SampleCountFlagBits::e1;

    _color_attachment_formats.clear();
    _depth_attachment_format       = vk::Format::eUndefined;

    _color_attachments.clear();
    _depth_attachment = vk::RenderingAttachmentInfo { };

    _destroy_depth_buffer();
    _destroy_multisample_buffer();

    _rendering_info       = vk::RenderingInfo { };
}

} // namespace vkl