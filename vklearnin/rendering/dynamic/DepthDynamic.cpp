#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/dynamic/DepthDynamic.hpp"

#include "vklearnin/vulkan/swapchain/vkSurface.hpp"
#include "vklearnin/vulkan/devices/vkDevice.hpp"

namespace vkl {

// =============================================================================
void DepthDynamic::init(vkSurface const &surface,
                        std::span<vk::ClearValue const> const clear_values,
                        vk::Format const depth_format,
                        vkDevice const &device)
{
    if(!device.native()) {
        Log::error("Cannot create depth dynamic with invalid device.");
        return;
    }

    if(!surface.native()) {
        Log::error("Cannot create depth dynamic with invalid surface.");
        return;
    }

    if(depth_format == vk::Format::eUndefined) {
        Log::error("Cannot create depth dynamic with undefined depth format.");
        return;
    }

    _color_attachment_formats = { surface.format().format };
    _depth_attachment_format = depth_format;

    if(!_create_depth_buffer(surface, device)) {
        Log::error("Failed to create depth dynamic depth buffer.");
        _reset_object();
        return;
    }

    _init_attachments(clear_values);
    _init_rendering_info(surface);
}

// =============================================================================
void DepthDynamic::shutdown() {
    _reset_object();
}

// =============================================================================
vk::RenderingInfo const &
DepthDynamic::rendering_info(vk::ImageView const &view,
                             vk::ImageLayout const &layout)
{
    _color_attachments[0].imageView = view;
    _color_attachments[0].imageLayout = layout;

    _depth_attachment.imageLayout = _depth_buffer.layout();

    return _rendering_info;
}

// =============================================================================
void DepthDynamic::update_render_area(vkSurface const &surface) {
    _rendering_info.renderArea = vk::Rect2D {
        .offset = vk::Offset2D { },
        .extent = surface.extent(),
    };
}

// =============================================================================
void DepthDynamic::destroy_swapchain_resources() {
    _rendering_info.renderArea = vk::Rect2D { };

    _color_attachment_formats.clear();
    _depth_attachment_format = vk::Format::eUndefined;

    _destroy_depth_buffer();
}

// =============================================================================
void DepthDynamic::create_swapchain_resources(
    vkSurface const &surface,
    std::span<vk::ClearValue const> const clear_values,
    vk::Format const depth_format,
    vkDevice const &device)
{
    _color_attachment_formats = { surface.format().format };
    _depth_attachment_format = depth_format;

    update_render_area(surface);
    _create_depth_buffer(surface, device);

    _init_attachments(clear_values);
    _init_rendering_info(surface);
}

// =============================================================================
void DepthDynamic::_init_attachments(
    std::span<vk::ClearValue const> const clear_values)
{
    _color_attachments = {{ vk::RenderingAttachmentInfo {
        .pNext = nullptr,
        .imageView = { },
        .imageLayout = { },
        .resolveMode = { },
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
void DepthDynamic::_init_rendering_info(vkSurface const &surface) {
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
DepthDynamic::_create_depth_buffer(vkSurface const &surface,
                                   vkDevice const &device)
{
    vkImage::Details const details {
        .type         = vk::ImageType::e2D,
        .samples      = vk::SampleCountFlagBits::e1,
        .usage_flags  = vk::ImageUsageFlagBits::eDepthStencilAttachment,
        .memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
    };

    if(!_depth_buffer.create(surface.extent(),
                             _depth_attachment_format,
                             details,
                             device))
    {
        Log::error("Failed to create depth dynamic depth buffer.");
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
        Log::error("Failed to create depth dynamic depth view.");
        return false;
    }

    return true;
}

// =============================================================================
void DepthDynamic::_destroy_depth_buffer() {
    if(_depth_view.native()) {
        _depth_view.destroy();
    }

    if(_depth_buffer.native()) {
        _depth_buffer.destroy();
    }
}

// =============================================================================
void DepthDynamic::_reset_object() {
    _color_attachment_formats.clear();
    _depth_attachment_format = vk::Format::eUndefined;

    _color_attachments.clear();
    _depth_attachment = vk::RenderingAttachmentInfo { };

    _destroy_depth_buffer();

    _rendering_info = vk::RenderingInfo { };
}

} // namespace vkl