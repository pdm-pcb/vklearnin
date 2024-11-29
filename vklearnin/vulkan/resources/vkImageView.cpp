#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/resources/vkImageView.hpp"

#include "vklearnin/vulkan/resources/vkImage.hpp"
#include "vklearnin/vulkan/devices/vkDevice.hpp"

namespace vkl {

// =============================================================================
vkImageView::vkImageView(vkImageView &&other) :
    _handle { other._handle },
    _device { other._device }
{
    other._handle = nullptr;
    other._device = nullptr;
}

// =============================================================================
bool vkImageView::create(Details const &details, vkDevice const &device) {
    if(_handle) {
        Log::error("Image view {} already exists", _handle);
        return false;
    }

    if(!details.image) {
        Log::error("Cannot create image view for invalid image.");
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot create image view with invalid device.");
        return false;
    }

    _device = device.native();

    vk::ImageViewCreateInfo const view_create_info {
        .image    = details.image,
        .viewType = details.type,
        .format   = details.format,
        .components = {
            .r = vk::ComponentSwizzle::eR,  // If color channel values are
            .g = vk::ComponentSwizzle::eG,  // swapped for some reason, these
            .b = vk::ComponentSwizzle::eB,  // paremeters allow us to specify
            .a = vk::ComponentSwizzle::eA,  // which should go where.
        },
        .subresourceRange {
            .aspectMask = details.aspect_flags, // Aspect flags describe
                                                // suitable interpretations
                                                // for the image data in memory

            .baseMipLevel   = 0u, // Starting mip level
            .levelCount     = 1u, // Total mip levels
            .baseArrayLayer = 0u, // Starting array layer
            .layerCount     = 1u, // Total array layers
        }
    };

    auto const [ result, value ] = _device.createImageView(view_create_info);
    if(result != vk::Result::eSuccess) {
        Log::error(
            "Failed to create image view: '{}'",
            vk::to_string(result)
        );
        return false;
    }

    _handle = value;
    Log::trace("Created view {} for image {}", _handle, details.image);

    return true;
}

// =============================================================================
bool vkImageView::destroy() {
    if(!_handle) {
        Log::error("Must create image view before calling destroy.");
        return false;
    }

    Log::trace("Destroying image view {}", _handle);
    _device.destroyImageView(_handle);
    _handle = nullptr;
    _device = nullptr;

    return true;
}

} // namespace vkl