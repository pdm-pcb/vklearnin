#include "vklearnin/vklearnin.hpp"
#include "vklearnin/resources/images/Texture2D.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void Texture2D::init_from_file(std::string_view filepath) {
    void *image_data = ImageTools::load_from_file(_image, filepath);

    ImageTools::create(
        _image,
        vk::ImageType::e2D,
        vk::SampleCountFlagBits::e1,
        (
            vk::ImageUsageFlagBits::eSampled |
            vk::ImageUsageFlagBits::eTransferDst
        ),
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    ImageTools::host_to_device(
        _image,
        image_data
    );

    ImageTools::free_file_data(image_data);

    ImageTools::create_view(
        _image,
        vk::ImageViewType::e2D,
        vk::ImageAspectFlagBits::eColor
    );
}

void Texture2D::init_sampler(const vk::Filter min_filter,
                             const vk::Filter mag_filter,
                             const vk::SamplerAddressMode mode_u,
                             const vk::SamplerAddressMode mode_v)
{
    ImageTools::create_sampler(_image, min_filter, mag_filter, mode_u, mode_v);
}

// =============================================================================
void Texture2D::shutdown() {
    ImageTools::destroy(_image);
}

// =============================================================================
Texture2D::Texture2D() :
    _image { }
{ }

} // namespace vkl