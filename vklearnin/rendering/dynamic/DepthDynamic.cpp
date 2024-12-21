#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/dynamic/DepthDynamic.hpp"

#include "vklearnin/vulkan/swapchain/vkSurface.hpp"
#include "vklearnin/vulkan/devices/vkPhysicalDevice.hpp"
#include "vklearnin/vulkan/devices/vkDevice.hpp"

namespace vkl {

// =============================================================================
void DepthDynamic::init(vkSurface const &surface,
                        std::span<vk::ClearValue const> const clear_values,
                        vkPhysicalDevice const &physical_device)
{
    if(!surface.native()) {
        Log::error("Cannot create color dynamic with invalid surface.");
        return;
    }

    if(!_find_depth_format(physical_device)) {
        Log::error("Unable to find suitable depth format.");
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
        .pDepthAttachment = { },
        .pStencilAttachment = { },
    };

    _pipeline_create_info = vk::PipelineRenderingCreateInfoKHR {
        .pNext = nullptr,
        .viewMask = { },
        .colorAttachmentCount =
            static_cast<uint32_t>(_color_attachment_formats.size()),
        .pColorAttachmentFormats = _color_attachment_formats.data(),
        .depthAttachmentFormat = { },
        .stencilAttachmentFormat = { },
    };
}



// =============================================================================
vk::RenderingInfoKHR const &
DepthDynamic::rendering_info(vk::ImageView const &view,
                             vk::ImageLayout const &layout)
{
    _color_attachments[0].imageView = view;
    _color_attachments[0].imageLayout = layout;

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
bool DepthDynamic::_find_depth_format(vkPhysicalDevice const &physical_device) {
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
bool
DepthDynamic::_create_depth_buffer(vkSurface const &surface,
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
void DepthDynamic::_destroy_depth_buffer() {
    _depth_view.destroy();
    _depth_buffer.destroy();
}

} // namespace vkl