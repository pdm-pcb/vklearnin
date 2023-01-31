#include "vklearnin/vklearnin.hpp"
#include "vklearnin/resources/images/ImageTools.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl::ImageTools {

// =============================================================================
void create_image(ImageObject &image, vk::ImageType type, vk::Extent3D extent,
                  vk::ImageUsageFlags usage_flags)
{
    if(image.format == vk::Format::eUndefined) {
        CONSOLE_CRITICAL("Cannot create image with undefined format.");
    }

    const vk::ImageCreateInfo image_info {
        .imageType   = type,
        .format      = image.format,
        .extent      = extent,
        .mipLevels   = 1u,
        .arrayLayers = 1u,
        .samples     = vk::SampleCountFlagBits::e1,
        .tiling      = vk::ImageTiling::eOptimal,
        .usage       = usage_flags
    };

    auto [result, handle] = LogicalDevice::native().createImage(image_info);
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Could not create image: '{}'",
            to_string(result)
        );
    }
    else {
        CONSOLE_TRACE("Created image {:#x}",
                      reinterpret_cast<uint64_t>(::VkImage(handle)));
    }
    
    image.handle = handle;
}

// =============================================================================
void destroy_image(ImageObject &image) {
    CONSOLE_TRACE("Destroying image {:#x}",
                   reinterpret_cast<uint64_t>(::VkImage(image.handle)));
    LogicalDevice::native().destroyImage(image.handle);

    image.view = nullptr;
}

// =============================================================================
void create_view(ImageObject &image,
                 const vk::ImageViewType view_type,
                 const vk::ImageAspectFlags &aspect_flags) {
    if(!image.handle) {
        CONSOLE_CRITICAL("Cannot create view for non-existant image.");
    }
    if(image.format == vk::Format::eUndefined) {
        CONSOLE_CRITICAL("Cannot create view for image with undefined format.");
    }

    const vk::ImageViewCreateInfo view_info {
        .image    = image.handle,
        .viewType = view_type,
        .format   = image.format,
        .components = {                     
            .r = vk::ComponentSwizzle::eR,  // If color channel values are
            .g = vk::ComponentSwizzle::eG,  // swapped for some reason, these
            .b = vk::ComponentSwizzle::eB,  // paremeters allow us to specify
            .a = vk::ComponentSwizzle::eA,  // which should go where.
        },
        .subresourceRange {
            .aspectMask = aspect_flags, // Aspect flags describe suitable
                                        // interpretations for this image's
                                        // data
            .baseMipLevel   = 0u, // Starting mip level
            .levelCount     = 1u, // Total mip levels
            .baseArrayLayer = 0u, // Starting array layer
            .layerCount     = 1u  // Total array layers
        }
    };

    auto [result, view] = LogicalDevice::native().createImageView(view_info);
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
    
    image.view = view;
}

// =============================================================================
void destroy_view(ImageObject &image) {
    CONSOLE_TRACE("Destroying image view {:#x}",
                   reinterpret_cast<uint64_t>(::VkImageView(image.view)));
    LogicalDevice::native().destroyImageView(image.view);

    image.view = nullptr;
}

} // namespace vkl::ImageTools