#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/targets/ColorBuffer.hpp"

namespace vkl {

// =============================================================================
void ColorBuffer::create(vk::Rect2D const &render_area,
                         vk::Format const format,
                         vk::SampleCountFlagBits const samples)
{
    if(render_area.extent.height == 0u || render_area.extent.width == 0u) {
        CONSOLE_CRITICAL("Cannot create color buffer of zero size.");
        return;
    }

    if(_image.handle) {
        CONSOLE_CRITICAL("Destroy existing color buffer.");
        return;
    }

    _render_area = render_area;

    _image.extent = vk::Extent3D {
        .width  = _render_area.extent.width,
        .height = _render_area.extent.height,
        .depth  = 1u
    };
    _image.format = format;
    _image.aspect_flags = vk::ImageAspectFlagBits::eColor;

    ImageTools::create(
        _image,
        vk::ImageType::e2D,
        samples,
        (
            vk::ImageUsageFlagBits::eColorAttachment |
            vk::ImageUsageFlagBits::eTransientAttachment
        ),
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    ImageTools::create_view(_image, vk::ImageViewType::e2D);
}

// =============================================================================
void ColorBuffer::destroy() {
    ImageTools::destroy(_image);
}

// =============================================================================
ColorBuffer::ColorBuffer() :
    _render_area { },
    _image { }
{ }

ColorBuffer::ColorBuffer(ColorBuffer &&other) noexcept :
    _render_area { std::move(other._render_area) },
    _image { std::move(other._image) }
{
    other._render_area = vk::Rect2D { };
    other._image = ImageObject { };
}

} // namespace vkl