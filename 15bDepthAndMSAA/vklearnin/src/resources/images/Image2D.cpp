#include "vklearnin/vklearnin.hpp"
#include "vklearnin/resources/images/Image2D.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void Image2D::init(const vk::Image image, const vk::Format format,
                   const vk::ImageLayout layout, const vk::Extent2D &extent)
{
    _image.handle = image;
    _image.format = format;
    _image.layout = layout;
    _extent       = extent;

    CONSOLE_TRACE(
        "\nStoring image {:#x}:"
        "\n    Extent: {}x{}"
        "\n    Format: {}"
        "\n    Layout: {}",
        reinterpret_cast<uint64_t>(::VkImage(_image.handle)),
        _extent.width, _extent.height,
        to_string(_image.format),
        to_string(_image.layout)
    );
}

// =============================================================================
Image2D::Image2D() :
    _image  { },
    _extent { 0u, 0u }
{ }

} // namespace vkl