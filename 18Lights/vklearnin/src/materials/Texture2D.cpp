#include "vklearnin/vklearnin.hpp"
#include "vklearnin/materials/Texture2D.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void Texture2D::texture_from_file(Filepath filepath) {
    void *image_data = ImageTools::image_from_file(_image, filepath);

    _calc_mip_levels();

    CONSOLE_TRACE(
        "\nCreating image with"
        "\n\tSize:         {} bytes"
        "\n\tExtent:       {}x{}x{}"
        "\n\tMip Levels:   {}"
        "\n\tArray Layers: {}",
        _image.size,
        _image.extent.width, _image.extent.height, _image.extent.depth,
        _image.mip_levels,
        _image.array_layers
    );

    ImageTools::create(
        _image,
        vk::ImageType::e2D,
        vk::ImageAspectFlagBits::eColor,
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

    ImageTools::free_image_data(image_data);

    ImageTools::create_view(
        _image,
        vk::ImageViewType::e2D,
        vk::ImageAspectFlagBits::eColor
    );
}

// =============================================================================
void Texture2D::cubemap_from_files(CubeFilepaths const &filepaths) {
    void *image_data = ImageTools::cubemap_from_files(_image, filepaths);

    _calc_mip_levels();

    CONSOLE_TRACE(
        "\nCreating image with"
        "\n\tSize:         {} bytes"
        "\n\tExtent:       {}x{}x{}"
        "\n\tMip Levels:   {}"
        "\n\tArray Layers: {}",
        _image.size,
        _image.extent.width, _image.extent.height, _image.extent.depth,
        _image.mip_levels,
        _image.array_layers
    );

    ImageTools::create(
        _image,
        vk::ImageType::e2D,
        vk::ImageAspectFlagBits::eColor,
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

    ImageTools::create_view(
        _image,
        vk::ImageViewType::eCube,
        vk::ImageAspectFlagBits::eColor
    );
}

// =============================================================================
void Texture2D::create_shadow_map(vk::Extent2D const &extent,
                                  vk::Format const depth_format)
{
    _image.format = depth_format;
    _image.extent = vk::Extent3D {
        .width  = extent.width,
        .height = extent.height,
        .depth  = 1u
    };

    ImageTools::create(
        _image,
        vk::ImageType::e2D,
        vk::ImageAspectFlagBits::eDepth,
        // No multisampling for shadow maps in favor of PCF/soft shadows
        vk::SampleCountFlagBits::e1,
        ( vk::ImageUsageFlagBits::eDepthStencilAttachment |
          vk::ImageUsageFlagBits::eSampled ),
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    _image.layout = vk::ImageLayout::eDepthStencilReadOnlyOptimal;

    ImageTools::create_view(
        _image,
        vk::ImageViewType::e2D,
        vk::ImageAspectFlagBits::eDepth
    );
}

// =============================================================================
void Texture2D::create_sampler(vk::Filter const min_filter,
                               vk::Filter const mag_filter,
                               vk::SamplerMipmapMode const mip_filter,
                               vk::SamplerAddressMode const mode_u,
                               vk::SamplerAddressMode const mode_v,
                               vk::Bool32 const enable_compare,
                               vk::CompareOp const compare_op)
{
    ImageTools::create_sampler(
        _image,
        min_filter, mag_filter,
        mip_filter,
        mode_u, mode_v,
        enable_compare, compare_op
    );
}

// =============================================================================
void Texture2D::generate_mipmap(vk::Filter const mip_filter) {
    ImageTools::generate_mipmap(_image, mip_filter);
}

// =============================================================================
void Texture2D::destroy() {
    ImageTools::destroy(_image);
}

// =============================================================================
void Texture2D::_calc_mip_levels() {
    auto const longest_side = std::max(
        static_cast<float>(_image.extent.width),
        static_cast<float>(_image.extent.height)
    );

    auto const mip_levels =
        static_cast<uint32_t>(std::floor(std::log2(longest_side))) + 1u;

    _image.mip_levels = mip_levels;
}

// =============================================================================
Texture2D::Texture2D() :
    _image { }
{ }

} // namespace vkl