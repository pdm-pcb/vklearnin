#include "vklearnin/vklearnin.hpp"
#include "vklearnin/tools/ImageTools.hpp"

namespace vkl {
namespace ImageTools {

// =============================================================================
vk::ImageView create_view(const vk::Image &image,
                          const vk::Format &color_format,
                          const vk::ImageAspectFlags &aspect_flags,
                          const vk::Device &device)
{
    vk::ImageViewCreateInfo image_info {
        .image = image,
        .viewType = vk::ImageViewType::e2D,
        .format = color_format,
        .components = {
            .r = vk::ComponentSwizzle::eR,
            .g = vk::ComponentSwizzle::eG,
            .b = vk::ComponentSwizzle::eB,
            .a = vk::ComponentSwizzle::eA,
        },
        .subresourceRange {
            .aspectMask     = aspect_flags,
            .baseMipLevel   = 0u,
            .levelCount     = 1u,
            .baseArrayLayer = 0u,
            .layerCount     = 1u
        }
    };

    auto [result, view] = device.createImageView(image_info);
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Could not create image view");
    }
    else {
        CONSOLE_TRACE("Created image view {:#x}",
                      reinterpret_cast<uint64_t>(::VkImageView(view)));
    }
    return view;
}

// =============================================================================
void destroy_view(vk::ImageView &view, const vk::Device &device)
{
    CONSOLE_TRACE("Destroy image view {:#x}",
                   reinterpret_cast<uint64_t>(::VkImageView(view)));
    device.destroy(view);
}

} // namespace ImageTools
} // namespace vkl