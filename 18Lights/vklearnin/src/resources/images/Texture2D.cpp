#include "vklearnin/vklearnin.hpp"
#include "vklearnin/resources/images/Texture2D.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void Texture2D::create(FilePath filepath, bool use_mipmaps) {
    void *image_data = ImageTools::image_from_file(_image, filepath);

    CONSOLE_TRACE(
        "\nCreating 2D texture with"
        "\n\tSize:         {} bytes"
        "\n\tExtent:       {}x{}x{}"
        "\n\tArray Layers: {}",
        _image.size,
        _image.extent.width, _image.extent.height, _image.extent.depth,
        _image.array_layers
    );

    _image.aspect_flags = vk::ImageAspectFlagBits::eColor;

    ImageTools::create(
        _image,
        vk::ImageType::e2D,
        use_mipmaps,
        vk::SampleCountFlagBits::e1,
        (
            vk::ImageUsageFlagBits::eSampled |
            vk::ImageUsageFlagBits::eTransferDst |
            vk::ImageUsageFlagBits::eTransferSrc // Required to create a mipmap
        ),
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    ImageTools::host_to_device(
        _image,
        image_data
    );

    ImageTools::create_view(_image, vk::ImageViewType::e2D);

    ImageTools::free_image_data(image_data);
}

// // =============================================================================
// void Texture2D::create_shadow_map(vk::Extent2D const &extent,
//                                   vk::Format const depth_format)
// {
//     _image.extent = vk::Extent3D {
//         .width  = extent.width,
//         .height = extent.height,
//         .depth  = 1u
//     };
//     _image.format = depth_format;
//     _image.layout = vk::ImageLayout::eDepthStencilReadOnlyOptimal;
//     _image.aspect_flags = vk::ImageAspectFlagBits::eDepth;

//     ImageTools::create(
//         _image,
//         vk::ImageType::e2D,
//         // No multisampling for shadow maps in favor of PCF/soft shadows
//         vk::SampleCountFlagBits::e1,
//         ( vk::ImageUsageFlagBits::eDepthStencilAttachment |
//           vk::ImageUsageFlagBits::eSampled ),
//         vk::MemoryPropertyFlagBits::eDeviceLocal
//     );

//     ImageTools::create_view(_image, vk::ImageViewType::e2D);
// }

// =============================================================================
void Texture2D::destroy() {
    ImageTools::destroy(_image);
}

// =============================================================================
Texture2D::Texture2D() :
    _image { }
{ }

Texture2D::Texture2D(Texture2D &&other) noexcept :
    _image { std::move(other._image) }
{
    other._image = ImageObject { };
}

Texture2D& Texture2D::operator=(Texture2D &&other) noexcept {
    _image = std::move(other._image);
    other._image = ImageObject { };

    return *this;
}

} // namespace vkl