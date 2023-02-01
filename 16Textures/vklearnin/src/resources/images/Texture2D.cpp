#include "vklearnin/vklearnin.hpp"
#include "vklearnin/resources/images/Texture2D.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void Texture2D::init_from_file(std::string_view filepath) {
    void *image_data = ImageTools::load_from_file(filepath, _image, _extent);

    auto extent3D = vk::Extent3D {
        .width  = _extent.width,
        .height = _extent.height,
        .depth  = 1u
    };

    ImageTools::create_image(
        _image,
        vk::ImageType::e2D,
        extent3D,
        vk::SampleCountFlagBits::e1,
        (
            vk::ImageUsageFlagBits::eSampled |
            vk::ImageUsageFlagBits::eTransferDst
        ),
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );
    
    ImageTools::host_to_device(
        _image,
        extent3D,
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
    _sampler = ImageTools::create_sampler(
        _image,
        min_filter,
        mag_filter,
        mode_u,
        mode_v
    );
}

// =============================================================================
void Texture2D::shutdown() {
    ImageTools::destroy_sampler(_sampler);
    ImageTools::destroy_image(_image);
}

// =============================================================================
Texture2D::Texture2D() :
    _image  { },
    _extent { 0u, 0u }
{ }

} // namespace vkl