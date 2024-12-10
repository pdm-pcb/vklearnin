#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/dynamic/ColorDynamic.hpp"

#include "vklearnin/vulkan/swapchain/vkSurface.hpp"
#include "vklearnin/vulkan/devices/vkDevice.hpp"
#include "vklearnin/vulkan/devices/vkCmdBuffer.hpp"

namespace vkl {

// =============================================================================
void ColorDynamic::init(vkSurface const &surface,
                        std::span<vk::ClearValue const> const clear_values)
{
    _color_attachments = {{ vk::RenderingAttachmentInfoKHR {
        .pNext = nullptr,
        .imageView = { },
        .imageLayout = { },
        .resolveMode = { },
        .resolveImageView = { },
        .resolveImageLayout = { },
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = clear_values[0].color,
    }}};

    _color_attachment_formats = {{ surface.format().format }};

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
ColorDynamic::rendering_info(vk::ImageView const &view,
                             vk::ImageLayout const &layout)
{
    _color_attachments[0].imageView = view;
    _color_attachments[0].imageLayout = layout;

    return _rendering_info;
}

// =============================================================================
void ColorDynamic::update_render_area(vkSurface const &surface) {
    _rendering_info.renderArea = vk::Rect2D {
        .offset = vk::Offset2D { },
        .extent = surface.extent(),
    };
}

} // namespace vkl