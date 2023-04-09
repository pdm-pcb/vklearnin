#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/FrameBuffer.hpp"

#include "vklearnin/system/devices/PhysicalDevice.hpp"
#include "vklearnin/rendering/swapchain/Swapchain.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"
#include "vklearnin/system/devices/CmdBuffer.hpp"

namespace vkl {

// =============================================================================
FrameBuffer &
FrameBuffer::create_color_buffer(vk::Extent2D const &extent,
                                 vk::SampleCountFlagBits const &samples)
{
    if(extent.height == 0u || extent.width == 0u) {
        CONSOLE_CRITICAL("Cannot create a color buffer with a zero extent.");
    }

    if(_color_buffer.handle) {
        CONSOLE_CRITICAL("Destroy this framebuffer before adding attachemnts.");
    }

    _color_buffer.extent = vk::Extent3D {
        .width  = extent.width,
        .height = extent.height,
        .depth  = 1u
    };
    _color_buffer.format = Swapchain::image_format();
    _color_buffer.aspect_flags = vk::ImageAspectFlagBits::eColor;

    ImageTools::create(
        _color_buffer,
        vk::ImageType::e2D,
        samples,
        (
            vk::ImageUsageFlagBits::eColorAttachment |
            vk::ImageUsageFlagBits::eTransientAttachment
        ),
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    ImageTools::create_view(_color_buffer, vk::ImageViewType::e2D);

    _attachments.emplace_back(_color_buffer.view);
    CONSOLE_TRACE(
        "FrameBuffer attachment {}: image view {:#x}",
        _attachments.size(),
        reinterpret_cast<uint64_t>(VkImageView(_attachments.back()))
    );

    return *this;
}

// =============================================================================
FrameBuffer &
FrameBuffer::create_depth_buffer(vk::Extent2D const &extent,
                                 vk::SampleCountFlagBits const &samples)
{
    if(extent.height == 0u || extent.width == 0u) {
        CONSOLE_CRITICAL("Cannot create a depth buffer with a zero extent.");
    }

    if(_depth_buffer.handle) {
        CONSOLE_CRITICAL("Destroy this framebuffer before adding attachemnts.");
    }

    _depth_buffer.extent = vk::Extent3D {
        .width  = extent.width,
        .height = extent.height,
        .depth  = 1u
    };
    _depth_buffer.format = PhysicalDevice::depth_format();
    _depth_buffer.aspect_flags = vk::ImageAspectFlagBits::eDepth;

    ImageTools::create(
        _depth_buffer,
        vk::ImageType::e2D,
        samples,
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    ImageTools::create_view(_depth_buffer, vk::ImageViewType::e2D);

    _attachments.emplace_back(_depth_buffer.view);
    CONSOLE_TRACE(
        "FrameBuffer attachment {}: image view {:#x}",
        _attachments.size(),
        reinterpret_cast<uint64_t>(VkImageView(_attachments.back()))
    );

    return *this;
}

// =============================================================================
FrameBuffer & FrameBuffer::add_image_view(vk::ImageView const &view) {
    _attachments.emplace_back(view);
    CONSOLE_TRACE(
        "FrameBuffer attachment {}: image view {:#x}",
        _attachments.size(),
        reinterpret_cast<uint64_t>(VkImageView(_attachments.back()))
    );

    return *this;
}

// =============================================================================
FrameBuffer & FrameBuffer::create_shadow_map(vk::Extent2D const &extent) {
    if(extent.height == 0u || extent.width == 0u) {
        CONSOLE_CRITICAL("Cannot create a shadow buffer with a zero extent.");
    }

    if(_shadow_map.image().handle) {
        CONSOLE_CRITICAL("Destroy this framebuffer before adding attachemnts.");
    }

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

    _attachments.emplace_back(_shadow_map.image().view);
    CONSOLE_TRACE(
        "FrameBuffer attachment {}: image view {:#x}",
        _attachments.size(),
        reinterpret_cast<uint64_t>(VkImageView(_attachments.back()))
    );

    return *this;
}

// =============================================================================
void FrameBuffer::create(vk::Rect2D const &render_area) {
    _render_area = render_area;
}

// =============================================================================
void FrameBuffer::destroy() {
    if(_depth_buffer.handle) {
        ImageTools::destroy(_depth_buffer);
    }
    if(_color_buffer.handle) {
        ImageTools::destroy(_color_buffer);
    }

    _shadow_map.destroy();
}

// =============================================================================
FrameBuffer::FrameBuffer() :
    _attachments  { },
    _color_buffer { },
    _depth_buffer { },
    _shadow_map   { },
    _render_area  { }
{ }

FrameBuffer::FrameBuffer(FrameBuffer &&other) noexcept :
    _attachments  { std::move(other._attachments)  },
    _color_buffer { std::move(other._color_buffer) },
    _depth_buffer { std::move(other._depth_buffer) },
    _shadow_map   { std::move(other._shadow_map)   },
    _render_area  { std::move(other._render_area)  }
{
    other._attachments.clear();
    other._color_buffer = ImageObject { };
    other._depth_buffer = ImageObject { };
    other._shadow_map   = Texture2D { };
}

} // namespace vkl