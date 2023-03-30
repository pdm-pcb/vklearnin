#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/renderpass/Framebuffer.hpp"

#include "vklearnin/system/devices/PhysicalDevice.hpp"
#include "vklearnin/rendering/renderpass/RenderPass.hpp"
#include "vklearnin/rendering/swapchain/Swapchain.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
Framebuffer & Framebuffer::create_color_buffer(vk::Extent2D const &extent) {
    if(extent.height == 0u || extent.width == 0u) {
        CONSOLE_CRITICAL("Cannot create a color buffer with a zero extent.");
    }

    if(_color_buffer.handle) {
        CONSOLE_CRITICAL("Destroy this framebuffer before adding attachemnts.");
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

    _attachments.emplace_back(_color_buffer.view);
    CONSOLE_TRACE(
        "Framebuffer attachment {}: image view {:#x}",
        _attachments.size(),
        reinterpret_cast<uint64_t>(VkImageView(_attachments.back()))
    );

    return *this;
}

// =============================================================================
Framebuffer & Framebuffer::create_depth_buffer(vk::Extent2D const &extent) {
    if(extent.height == 0u || extent.width == 0u) {
        CONSOLE_CRITICAL("Cannot create a depth buffer with a zero extent.");
    }

    if(_depth_buffer.handle) {
        CONSOLE_CRITICAL("Destroy this framebuffer before adding attachemnts.");
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

    _attachments.emplace_back(_depth_buffer.view);
    CONSOLE_TRACE(
        "Framebuffer attachment {}: image view {:#x}",
        _attachments.size(),
        reinterpret_cast<uint64_t>(VkImageView(_attachments.back()))
    );

    return *this;
}

// =============================================================================
Framebuffer & Framebuffer::add_image_view(vk::ImageView const &view) {
    _attachments.emplace_back(view);
    CONSOLE_TRACE(
        "Framebuffer attachment {}: image view {:#x}",
        _attachments.size(),
        reinterpret_cast<uint64_t>(VkImageView(_attachments.back()))
    );

    return *this;
}

// =============================================================================
Framebuffer & Framebuffer::create_shadow_map(vk::Extent2D const &extent) {
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
        "Framebuffer attachment {}: image view {:#x}",
        _attachments.size(),
        reinterpret_cast<uint64_t>(VkImageView(_attachments.back()))
    );

    return *this;
}

// =============================================================================
void Framebuffer::create(vk::Rect2D const &render_area,
                         vk::RenderPass const &render_pass)
{
    if(_attachments.empty()) {
        CONSOLE_CRITICAL("Cannot create a framebuffer with zero attachments.");
        return;
    }

    _render_area = render_area;

    const vk::FramebufferCreateInfo buffer_info {
        .renderPass      = render_pass,
        .attachmentCount = static_cast<uint32_t>(_attachments.size()),
        .pAttachments    = _attachments.data(),
        .width           = _render_area.extent.width,
        .height          = _render_area.extent.height,
        .layers          = 1u,
    };

    _framebuffer = LogicalDevice::native().createFramebuffer(buffer_info);

    CONSOLE_TRACE(
        "Created framebuffer {:#x} with {} attachments",
        reinterpret_cast<uint64_t>(VkFramebuffer(_framebuffer)),
        _attachments.size()
    );
}

// =============================================================================
void Framebuffer::destroy() {
    if(_depth_buffer.handle) {
        ImageTools::destroy(_depth_buffer);
    }
    if(_color_buffer.handle) {
        ImageTools::destroy(_color_buffer);
    }

    _shadow_map.destroy();

    CONSOLE_TRACE(
        "Destroying framebuffer {:#x}",
        reinterpret_cast<uint64_t>(VkFramebuffer(_framebuffer))
    );

    LogicalDevice::native().destroyFramebuffer(_framebuffer);
    _framebuffer = nullptr;
}

// =============================================================================
Framebuffer::Framebuffer() :
    _attachments  { },
    _color_buffer { },
    _depth_buffer { },
    _shadow_map   { },
    _render_area  { },
    _framebuffer  { }
{ }

Framebuffer::Framebuffer(Framebuffer &&other) noexcept :
    _attachments  { std::move(other._attachments)  },
    _color_buffer { std::move(other._color_buffer) },
    _depth_buffer { std::move(other._depth_buffer) },
    _shadow_map   { std::move(other._shadow_map)   },
    _render_area  { std::move(other._render_area)  },
    _framebuffer  { std::move(other._framebuffer)  }
{
    other._attachments.clear();
    other._color_buffer = ImageObject { };
    other._depth_buffer = ImageObject { };
    other._shadow_map   = Texture2D { };
    other._framebuffer  = nullptr;
}

} // namespace vkl