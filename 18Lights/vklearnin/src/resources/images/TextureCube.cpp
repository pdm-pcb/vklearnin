#include "vklearnin/vklearnin.hpp"
#include "vklearnin/resources/images/TextureCube.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void TextureCube::create(FilePaths const &filepaths, bool use_mipmaps) {
    void *image_data = ImageTools::cubemap_from_files(_image, filepaths);

    CONSOLE_TRACE(
        "\nCreating cube texture with"
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
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        vk::ImageCreateFlagBits::eCubeCompatible // Required cubemap flag
    );

    ImageTools::host_to_device(
        _image,
        image_data
    );

    ImageTools::free_cubemap_data(image_data);

    ImageTools::create_view(_image, vk::ImageViewType::eCube);
}

// =============================================================================
void TextureCube::destroy() {
    ImageTools::destroy(_image);
}

// =============================================================================
TextureCube::TextureCube() :
    _image { }
{ }

TextureCube::TextureCube(TextureCube &&other) noexcept :
    _image { std::move(other._image) }
{
    other._image = ImageObject { };
}

TextureCube& TextureCube::operator=(TextureCube &&other) noexcept {
    _image = std::move(other._image);
    other._image = ImageObject { };

    return *this;
}

} // namespace vkl