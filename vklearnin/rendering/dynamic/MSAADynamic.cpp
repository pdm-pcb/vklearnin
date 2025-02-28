/*

#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/dynamic/MSAADynamic.hpp"

#include "vklearnin/vulkan/swapchain/vkSurface.hpp"
#include "vklearnin/vulkan/devices/vkPhysicalDevice.hpp"
#include "vklearnin/vulkan/devices/vkDevice.hpp"

namespace vkl {

// =============================================================================
void MSAADynamic::init(vkSurface const &surface,
                       std::span<vk::ClearValue const> const clear_values,
                       vk::Format const depth_format,
                       vkPhysicalDevice const &physical_device,
                       vkDevice const &device)
{
    if(!surface.native()) {
        Log::error("Cannot create depth dynamic with invalid surface.");
        return;
    }

    if(depth_format == vk::Format::eUndefined) {
        Log::error("Cannot create depth dynamic with undefined depth format.");
        return;
    }

    _color_attachments = {{ vk::RenderingAttachmentInfoKHR {
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

    _color_attachment_formats = { surface.format().format };

    _depth_format = depth_format;

    if(!_create_depth_buffer(surface, physical_device, device)) {
        Log::error("Failed to create depth pass depth buffer.");

        _depth_format = vk::Format::eUndefined;
        _color_attachment_formats.clear();
        _color_attachments.clear();

        return;
    }

    _depth_attachment = vk::RenderingAttachmentInfoKHR {
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

    _rendering_info = vk::RenderingInfoKHR {
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

    _pipeline_create_info = vk::PipelineRenderingCreateInfoKHR {
        .pNext = nullptr,
        .viewMask = { },
        .colorAttachmentCount =
            static_cast<uint32_t>(_color_attachment_formats.size()),
        .pColorAttachmentFormats = _color_attachment_formats.data(),
        .depthAttachmentFormat = _depth_format,
        .stencilAttachmentFormat = _depth_format,
    };
}

// =============================================================================
void MSAADynamic::shutdown() {
    _depth_view.destroy();
    _depth_buffer.destroy();
}

// =============================================================================
vk::RenderingInfoKHR const &
MSAADynamic::rendering_info(vk::ImageView const &view,
                            vk::ImageLayout const &layout)
{
    _color_attachments[0].imageView = view;
    _color_attachments[0].imageLayout = layout;

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
bool MSAADynamic::_create_depth_buffer(vkSurface const &surface,
                                       vkPhysicalDevice const &physical_device,
                                       vkDevice const &device)
{
    vkImage::Details const details {
        .type         = vk::ImageType::e2D,
        .samples      = vk::SampleCountFlagBits::e1,
        .usage_flags  = vk::ImageUsageFlagBits::eDepthStencilAttachment,
        .memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
    };

    if(!_depth_buffer.create(
        surface.extent(),
        _depth_format,
        details,
        physical_device,
        device,
        "MSAADynamic depth buffer"
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
void MSAADynamic::_destroy_depth_buffer() {
    _depth_view.destroy();
    _depth_buffer.destroy();
}

} // namespace vkl

*/