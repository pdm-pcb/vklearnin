#include "vklearnin/vklearnin.hpp"
#include "vklearnin/textures/Texture2D.hpp"

#include "vklearnin/vulkan/descriptors/vkDescriptorPool.hpp"
#include "vklearnin/vulkan/devices/vkPhysicalDevice.hpp"
#include "vklearnin/vulkan/devices/vkDevice.hpp"
#include "vklearnin/vulkan/devices/vkCmdBuffer.hpp"

namespace vkl {

// =============================================================================
bool Texture2D::create(std::string_view const file_name,
                       vkImage::Details const &image_details,
                       vk::ImageViewType const view_type,
                       vkSampler::Filters const &sampler_filters,
                       vkSampler::AddressMode const &sampler_address_mode,
                       vkPhysicalDevice const &physical_device,
                       vkDevice const &device)
{
    if(_image.native()) {
        Log::error("Texture2D image {} already exists.", _image.native());
        return false;
    }

    if(!physical_device.native()) {
        Log::error("Cannot create Texture2D with invalid physical device.");
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot create Texture2D with invalid device.");
        return false;
    }

    if(!_image.create(
        file_name,
        image_details,
        physical_device,
        device
    ))
    {
        Log::error("Failed to create Texture2D image.");
        return false;
    }

    if(!_view.create(vkImageView::Details {
            .image = _image.native(),
            .format = _image.format(),
            .type = view_type,
            .aspect_flags = _image.aspect_flags()
        },
        device
    ))
    {
        Log::error("Failed to create Texture2D view.");
        _image.destroy();
        return false;
    }

    if(!_sampler.create(
        sampler_filters,
        sampler_address_mode,
        physical_device,
        device
    ))
    {
        Log::error("Failed to create Texture2D sampler.");
        _view.destroy();
        _image.destroy();
        return false;
    }

    return true;
}

// =============================================================================
bool Texture2D::destroy() {
    if(!_image.native()) {
        Log::error("Must create Texture2D before calling destroy.");
        return false;
    }

    if(!_sampler.destroy()) {
        Log::error("Failed to destroy Texture2D sampler.");
        return false;
    }
    if(!_view.destroy()) {
        Log::error("Failed to destroy Texture2D view.");
        return false;
    }
    if(!_image.destroy()) {
        Log::error("Failed to destroy Texture2D image.");
        return false;
    }

    return true;
}

} // namespace vkl