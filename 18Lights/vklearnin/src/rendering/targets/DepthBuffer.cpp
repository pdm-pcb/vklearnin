#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/targets/DepthBuffer.hpp"

namespace vkl {

// =============================================================================
void DepthBuffer::create(vk::Rect2D const &render_area,
                         vk::Format const format,
                         vk::SampleCountFlagBits const samples)
{
    if(render_area.extent.height == 0u || render_area.extent.width == 0u) {
        CONSOLE_CRITICAL("Cannot create depth buffer of zero size.");
        return;
    }

    if(_image.handle) {
        CONSOLE_CRITICAL("Destroy existing depth buffer.");
        return;
    }

    _render_area = render_area;

    _image.extent = vk::Extent3D {
        .width  = _render_area.extent.width,
        .height = _render_area.extent.height,
        .depth  = 1u
    };
    _image.format = format;
    _image.aspect_flags = vk::ImageAspectFlagBits::eDepth;

    ImageTools::create(
        _image,
        vk::ImageType::e2D,
        false,
        samples,
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    ImageTools::create_view(_image, vk::ImageViewType::e2D);
}

// =============================================================================
void DepthBuffer::destroy() {
    ImageTools::destroy(_image);
}

// =============================================================================
DepthBuffer::DepthBuffer() :
    _render_area { },
    _image { }
{ }

DepthBuffer::DepthBuffer(DepthBuffer &&other) noexcept :
    _render_area { std::move(other._render_area) },
    _image { std::move(other._image) }
{
    other._render_area = vk::Rect2D { };
    other._image = ImageObject { };
}

} // namespace vkl