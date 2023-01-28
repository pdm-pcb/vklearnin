#include "vklearnin/vklearnin.hpp"
#include "vklearnin/render/images/Image2D.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void Image2D::init(const vk::Image image, const vk::Format format,
                   const vk::ImageLayout layout, const vk::Extent2D &extent)
{
    _image  = image;
    _format = format;
    _layout = layout;
    _extent = extent;

    CONSOLE_TRACE(
        "\nStoring image {:#x}:"
        "\n    Extent: {}x{}"
        "\n    Format: {}"
        "\n    Layout: {}",
        reinterpret_cast<uint64_t>(::VkImage(_image)),
        _extent.width, _extent.height,
        to_string(_format),
        to_string(_layout)
    );
}

// =============================================================================
void Image2D::create_view(const vk::ImageAspectFlags &aspect_flags) {
    if(!_image) {
        CONSOLE_CRITICAL("Cannot create view for non-existant image.");
    }
    if(_format == vk::Format::eUndefined) {
        CONSOLE_CRITICAL("Cannot create view for image with undefined format.");
    }

    const vk::ImageViewCreateInfo image_info {
        .image = _image,                    // The image handle itself
        .viewType = vk::ImageViewType::e2D, // Image dimension count
        .format = _format,                  // Color format
        .components = {                     
            .r = vk::ComponentSwizzle::eR,  // If color channel values are
            .g = vk::ComponentSwizzle::eG,  // swapped for some reason, these
            .b = vk::ComponentSwizzle::eB,  // paremeters allow us to specify
            .a = vk::ComponentSwizzle::eA,  // which should go where.
        },
        .subresourceRange {
            .aspectMask     = aspect_flags, // Aspect flags describe suitable
                                            // interpretations for this image's
                                            // data
            .baseMipLevel   = 0u,   // Starting mip level
            .levelCount     = 1u,   // Total mip levels
            .baseArrayLayer = 0u,   // Starting array layer
            .layerCount     = 1u    // Total array layers
        }
    };

    auto [result, view] = LogicalDevice::native().createImageView(image_info);
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Could not create image view: '{}'",
            to_string(result)
        );
    }
    else {
        CONSOLE_TRACE("Created image view {:#x}",
                      reinterpret_cast<uint64_t>(::VkImageView(view)));
    }
    
    _view = view;
}

// =============================================================================
void Image2D::destroy_view() {
    CONSOLE_TRACE("Destroying image view {:#x}",
                   reinterpret_cast<uint64_t>(::VkImageView(_view)));
    LogicalDevice::native().destroy(_view);

    _view = nullptr;
}

// =============================================================================
Image2D::Image2D() :
    _image  { nullptr },
    _view   { nullptr },
    _format { vk::Format::eUndefined },
    _layout { vk::ImageLayout::eUndefined },
    _extent { 0u, 0u }
{ }

} // namespace vkl