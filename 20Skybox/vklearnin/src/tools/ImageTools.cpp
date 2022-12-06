#include "vklearnin/vklearnin.hpp"
#include "vklearnin/tools/ImageTools.hpp"

#include "vklearnin/rendering/devices/PhysicalDevice.hpp"
#include "vklearnin/rendering/devices/LogicalDevice.hpp"
#include "vklearnin/tools/VKAllocator.hpp"

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

namespace vkl {
namespace ImageTools {

// =============================================================================
ImageObject load_texture_from_file(const std::string_view &filepath,
                                   const bool flip_vertical)
{
    ImageObject result { };

    int width;
    int height;
    int channels;

    ::stbi_set_flip_vertically_on_load(flip_vertical);
    ::stbi_uc *image_data = ::stbi_load(
        filepath.data(),
        &width,
        &height,
        &channels,
        ::STBI_rgb_alpha
    );

    if(image_data == nullptr) {
		CONSOLE_CRITICAL("Failed to load image '{}'\n\t"
                         "Size/Channels: {}x{}@{}\n\t"
                         "Error: '{}'",
                         filepath, width, height, channels,
                         ::stbi_failure_reason());
        return result;
    }

    CONSOLE_TRACE("Loaded image '{}'", filepath);

    auto mip_levels = static_cast<uint32_t>(
        std::floor(std::log2(std::max(
            static_cast<float>(width), static_cast<float>(height)))
        )) + 1u;

    CONSOLE_TRACE("Set {} mip levels for '{}'", mip_levels, filepath);

    result = ImageTools::create_image(
        { },
        vk::Extent3D {
            .width  = static_cast<uint32_t>(width),
            .height = static_cast<uint32_t>(height),
            .depth  = 1u
        },
        static_cast<uint8_t>(::STBI_rgb_alpha),
        vk::Format::eR8G8B8A8Unorm,
        vk::ImageTiling::eOptimal,
        mip_levels,
        1u,
        vk::SampleCountFlagBits::e1,
        (vk::ImageUsageFlagBits::eTransferSrc |
         vk::ImageUsageFlagBits::eTransferDst |
         vk::ImageUsageFlagBits::eSampled),
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        "tex from file"
    );

    result.width = static_cast<uint32_t>(width);
    result.height = static_cast<uint32_t>(height);
    result.channels = static_cast<uint32_t>(::STBI_rgb_alpha);
    result.layer_size = result.width * result.height * result.channels;
    result.image_size = result.layer_size;

    create_view(
        result,
        vk::ImageViewType::e2D,
        vk::Format::eR8G8B8A8Unorm,
        vk::ImageAspectFlagBits::eColor
    );

    auto staging_buffer = BufferTools::stage_data(
        result.image_size,
        image_data
    );

    ImageTools::move_to_device(
        staging_buffer,
        result,
        1u,
        { result.width, result.height, 1u }
    );
    
    generate_mipmaps(result);

    ::stbi_image_free(image_data);
    BufferTools::destroy_buffer(staging_buffer);

    ImageTools::create_sampler(
        result,
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eClampToBorder
    );

    return result;
}

// =============================================================================
ImageObject
load_cubemap_from_files(const std::array<const std::string_view, 6> &filepaths,
                        const bool flip_vertical)
{
    ImageObject result { };

    int width = 0;
    int height = 0;
    int channels = 0;

    std::array<::stbi_uc *, 6> image_data { nullptr };
    auto current_image = image_data.begin();

    ::stbi_set_flip_vertically_on_load(flip_vertical);
    for(const auto &filepath : filepaths) {
        *current_image = ::stbi_load(
            filepath.data(),
            &width,
            &height,
            &channels,
            ::STBI_rgb_alpha
        );

        if(*current_image == nullptr) {
            CONSOLE_CRITICAL("Failed to load image '{}'\n\t"
                            "Size/Channels: {}x{}@{}\n\t"
                            "Error: '{}'",
                            filepath, width, height, channels,
                            ::stbi_failure_reason());
            return result;
        }

        CONSOLE_TRACE("Loaded image for cubemap '{}'", filepath);

        ++current_image;
    }

    auto mip_levels = static_cast<uint32_t>(
        std::floor(std::log2(std::max(
            static_cast<float>(width), static_cast<float>(height)))
        )) + 1u;

    // uint32_t mip_levels = 1u;

    CONSOLE_TRACE("Set {} mip levels for cubemap", mip_levels);

    result = ImageTools::create_image(
        vk::ImageCreateFlagBits::eCubeCompatible,
        vk::Extent3D {
            .width  = static_cast<uint32_t>(width),
            .height = static_cast<uint32_t>(height),
            .depth  = 1u
        },
        static_cast<uint8_t>(::STBI_rgb_alpha),
        vk::Format::eR8G8B8A8Unorm,
        vk::ImageTiling::eOptimal,
        mip_levels,
        static_cast<uint32_t>(filepaths.size()),
        vk::SampleCountFlagBits::e1,
        (vk::ImageUsageFlagBits::eTransferSrc |
         vk::ImageUsageFlagBits::eTransferDst |
         vk::ImageUsageFlagBits::eSampled),
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        "cubemap files"
    );

    result.width = static_cast<uint32_t>(width);
    result.height = static_cast<uint32_t>(height);
    result.channels = static_cast<uint32_t>(::STBI_rgb_alpha);
    result.layer_size = result.width * result.height * result.channels;
    result.image_size = result.layer_size * filepaths.size();

    create_view(
        result,
        vk::ImageViewType::eCube,
        vk::Format::eR8G8B8A8Unorm,
        vk::ImageAspectFlagBits::eColor
    );

    char *consolidated_image = new char[result.image_size];
    char *offset = consolidated_image;

    for(uint32_t image = 0u; image < filepaths.size(); ++image) {
        memcpy(offset, image_data[image], result.layer_size);
        offset += result.layer_size;
    }

    for(const auto &image : image_data) {
        ::stbi_image_free(image);
    }

    auto staging_buffer = BufferTools::stage_data(
        result.image_size,
        consolidated_image
    );

    ImageTools::move_to_device(
        staging_buffer,
        result,
        6u,
        { result.width, result.height, 1u }
    );
    
    generate_mipmaps(result);
    
    delete[] consolidated_image;
    BufferTools::destroy_buffer(staging_buffer);

    ImageTools::create_sampler(
        result,
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eClampToBorder
    );

    return result;
}

// =============================================================================
ImageObject create_image(const vk::ImageCreateFlags &flags,
                         const vk::Extent3D &extent,
                         const uint8_t channels,
                         const vk::Format &color_format,
                         const vk::ImageTiling &tiling,
                         const uint32_t mip_levels,
                         const uint32_t array_layers,
                         const vk::SampleCountFlagBits &sample_count,
                         const vk::ImageUsageFlags &usage,
                         const vk::MemoryPropertyFlags memory_properties,
                         const std::string_view &image_name)
{
    vk::ImageCreateInfo image_info {
        .flags       = flags,
        .imageType   = vk::ImageType::e2D,
        .format      = color_format,
        .extent      = extent,
        .mipLevels   = mip_levels,
        .arrayLayers = array_layers,
        .samples     = sample_count,
        .tiling      = tiling,
        .usage       = usage,
        .sharingMode = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 0u,
        .pQueueFamilyIndices = nullptr,
        .initialLayout = vk::ImageLayout::eUndefined
    };

    auto result = LogicalDevice::native().createImage(image_info);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to create image. {}",
                         to_string(result.result));
        return { };
    }

    CONSOLE_TRACE(
        "Created image {:#x}",
        reinterpret_cast<uint64_t>(VkImage(result.value))
    );

    ImageObject image_result {
        .image        = result.value,
        .format       = color_format,
        .width        = extent.width,
        .height       = extent.height,
        .channels     = channels,
        .mip_levels   = mip_levels,
        .array_layers = array_layers,
    };

    auto alloc = VKAllocator::allocate(
        result.value,
        memory_properties,
        image_name
    );
    image_result.allocation = alloc;
    
    return image_result;
}

// =============================================================================
void create_view(ImageObject &image, const vk::ImageViewType &type,
                 const vk::Format &color_format,
                 const vk::ImageAspectFlags &image_aspect)
{
    uint32_t layer_count = 1u;
    if(type == vk::ImageViewType::eCube) {
        layer_count = 6u;
    }

    vk::ImageViewCreateInfo image_info {
        .image = image.image,
        .viewType = type,
        .format = color_format,
        .components = {
            .r = vk::ComponentSwizzle::eR,
            .g = vk::ComponentSwizzle::eG,
            .b = vk::ComponentSwizzle::eB,
            .a = vk::ComponentSwizzle::eA,
        },
        .subresourceRange {
            .aspectMask     = image_aspect,
            .baseMipLevel   = 0u,
            .levelCount     = image.mip_levels,
            .baseArrayLayer = 0u,
            .layerCount     = layer_count
        }
    };

    auto [result, view] = LogicalDevice::native().createImageView(image_info);
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Could not create image view");
    }
    else {
        CONSOLE_TRACE("Created image view {:#x}",
                      reinterpret_cast<uint64_t>(::VkImageView(view)));
    }
    
    image.view = view;
}

// =============================================================================
void create_sampler(ImageObject &image,
                    const vk::Filter min_filter,
                    const vk::Filter mag_filter,
                    const vk::SamplerMipmapMode mip_filter,
                    const vk::SamplerAddressMode address_mode_u,
                    const vk::SamplerAddressMode address_mode_v,
                    const vk::SamplerAddressMode address_mode_w)
{
    vk::SamplerCreateInfo sampler_info {
        .magFilter        = min_filter,
        .minFilter        = mag_filter,
        .mipmapMode       = mip_filter,
        .addressModeU     = address_mode_u,
        .addressModeV     = address_mode_v,
        .addressModeW     = address_mode_w,
        .mipLodBias       = 0.0f,
        .anisotropyEnable = true,
        .maxAnisotropy    = RenderConfig::anisotropy,
        .compareEnable    = false,
        .compareOp        = vk::CompareOp::eAlways,
        .minLod           = 1.0f,
        .maxLod           = static_cast<float>(image.mip_levels),
        .borderColor      = vk::BorderColor::eIntOpaqueBlack,
        .unnormalizedCoordinates = false
    };

    auto result = LogicalDevice::native().createSampler(sampler_info);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to create image sampler");
    }
    else {
        CONSOLE_TRACE("Created image sampler {:#x}",
                      reinterpret_cast<uint64_t>(::VkSampler(result.value)));
    }

    image.sampler = result.value;
}

// =============================================================================
void move_to_device(const BufferObject &source, ImageObject &dest,
                    const uint32_t layer_count, const vk::Extent3D &extent)
{
    auto command_buffer = BufferTools::begin_oneshot_cmd_buffer();

    std::vector<vk::BufferImageCopy> copy_regions;
    copy_regions.reserve(layer_count);

    for(uint32_t layer = 0u; layer < layer_count; ++layer) {
        copy_regions.push_back({
            .bufferOffset = layer * dest.layer_size,
            .bufferRowLength = 0u,
            .bufferImageHeight = 0u,
            .imageSubresource {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .mipLevel = 0u,
                .baseArrayLayer = layer,
                .layerCount = 1u,
            },
            .imageOffset {
                .x = 0,
                .y = 0,
                .z = 0
            },
            .imageExtent = extent
        });

        CONSOLE_TRACE(
            "Layer {}, buffer offset: {}",
            layer,
            copy_regions.back().bufferOffset
        );
    }

    transition_layout(
        dest,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal,
        command_buffer,
        0u,
        dest.mip_levels,
        0u,
        layer_count
    );

    command_buffer.copyBufferToImage(
        source.buffer,
        dest.image,
        dest.layout,
        copy_regions
    );

    BufferTools::end_oneshot_cmd_buffer(command_buffer);
}

// =============================================================================
void ImageTools::generate_mipmaps(ImageObject &image) {
    auto format_props = PhysicalDevice::native().getFormatProperties(
        image.format
    );

    if(!(format_props.optimalTilingFeatures &
         vk::FormatFeatureFlagBits::eSampledImageFilterLinear))
    {
        CONSOLE_CRITICAL(
            "Image format {} does not support linear data blitting.",
            to_string(image.format)
        );
    }

    int32_t mip_width  = static_cast<int32_t>(image.width);
    int32_t mip_height = static_cast<int32_t>(image.height);

    auto command_buffer = BufferTools::begin_oneshot_cmd_buffer();

    for(uint32_t array_layer = 0u;
        array_layer < image.array_layers;
        ++array_layer)
    {
        CONSOLE_TRACE("Processing array layer {}", array_layer);

        for(uint32_t mip_level = 1u; mip_level < image.mip_levels; ++mip_level)
        {
            CONSOLE_TRACE("Generating mip level {}", mip_level);

            transition_layout(
                image,
                vk::ImageLayout::eTransferDstOptimal,
                vk::ImageLayout::eTransferSrcOptimal,
                command_buffer,
                mip_level - 1u,
                1u,
                array_layer,
                1u
            );

            vk::ImageBlit blit {
                .srcSubresource {
                    .aspectMask = vk::ImageAspectFlagBits::eColor,
                    .mipLevel = mip_level - 1,
                    .baseArrayLayer = array_layer,
                    .layerCount = 1u,
                },
                .srcOffsets = std::array<vk::Offset3D, 2> {
                    vk::Offset3D { 0, 0, 0 },
                    vk::Offset3D { mip_width, mip_height, 1 }
                },
                .dstSubresource {
                    .aspectMask = vk::ImageAspectFlagBits::eColor,
                    .mipLevel = mip_level,
                    .baseArrayLayer = array_layer,
                    .layerCount = 1u,
                },
                .dstOffsets = std::array<vk::Offset3D, 2> {
                    vk::Offset3D { 0, 0, 0 },
                    vk::Offset3D {
                        (mip_width  > 1 ? mip_width  / 2 : 1),
                        (mip_height > 1 ? mip_height / 2 : 1),
                        1
                    }
                },
            };

            command_buffer.blitImage(
                image.image,
                vk::ImageLayout::eTransferSrcOptimal,
                image.image,
                vk::ImageLayout::eTransferDstOptimal,
                { blit },
                vk::Filter::eLinear
            );

            transition_layout(
                image,
                vk::ImageLayout::eTransferSrcOptimal,
                vk::ImageLayout::eShaderReadOnlyOptimal,
                command_buffer,
                mip_level - 1u,
                1u,
                array_layer,
                1u
            );

            if(mip_width  > 1) { mip_width  /= 2; }
            if(mip_height > 1) { mip_height /= 2; }
        }

        transition_layout(
            image,
            vk::ImageLayout::eTransferDstOptimal,
            vk::ImageLayout::eShaderReadOnlyOptimal,
            command_buffer,
            image.mip_levels - 1u,
            1u,
            array_layer,
            1u
        );
    }

    BufferTools::end_oneshot_cmd_buffer(command_buffer);
}

// =============================================================================
void transition_layout(ImageObject &image,
                       const vk::ImageLayout old_layout,
                       const vk::ImageLayout new_layout,
                       const vk::CommandBuffer &command_buffer,
                       const uint32_t base_mip_level,
                       const uint32_t level_count,
                       const uint32_t base_array_layer,
                       const uint32_t layer_count)
{
    vk::ImageMemoryBarrier barrier {
        .oldLayout = old_layout,
        .newLayout = new_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image.image,
        .subresourceRange {
            .aspectMask     = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel   = base_mip_level,
            .levelCount     = level_count,
            .baseArrayLayer = base_array_layer,
            .layerCount     = layer_count,
        }
    };

    CONSOLE_TRACE(
        "Image {:#x}, mip {}/{}, layer {}/{}: '{}' -> '{}'",
        reinterpret_cast<uint64_t>(VkImage(image.image)),
        base_mip_level, level_count,
        base_array_layer, layer_count,
        to_string(barrier.oldLayout),
        to_string(barrier.newLayout)
    );

    vk::PipelineStageFlags source_stage      = vk::PipelineStageFlagBits::eNone;
    vk::PipelineStageFlags destination_stage = vk::PipelineStageFlagBits::eNone;

    if(barrier.oldLayout == vk::ImageLayout::eUndefined) {
        if(new_layout == vk::ImageLayout::eTransferDstOptimal) {
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            source_stage      = vk::PipelineStageFlagBits::eTopOfPipe;
            destination_stage = vk::PipelineStageFlagBits::eTransfer;
        }
        else {
            CONSOLE_CRITICAL("Unsupported image layout transition");
        }
    }
    else if(barrier.oldLayout == vk::ImageLayout::eTransferDstOptimal) {
        if(barrier.newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {    
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            source_stage      = vk::PipelineStageFlagBits::eTransfer;
            destination_stage = vk::PipelineStageFlagBits::eFragmentShader;
        }
        else if(barrier.newLayout == vk::ImageLayout::eTransferSrcOptimal) {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

            source_stage      = vk::PipelineStageFlagBits::eTransfer;
            destination_stage = vk::PipelineStageFlagBits::eTransfer;
        }
        else {
            CONSOLE_CRITICAL("Unsupported image layout transition");
        }
    }
    else if(barrier.oldLayout == vk::ImageLayout::eTransferSrcOptimal) {
        if(barrier.newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            source_stage      = vk::PipelineStageFlagBits::eTransfer;
            destination_stage = vk::PipelineStageFlagBits::eFragmentShader;
        }
        else {
            CONSOLE_CRITICAL("Unsupported image layout transition");
        }
    }
    else {
        CONSOLE_CRITICAL("Unsupported image layout transition");
    }

    vk::ImageMemoryBarrier image_barriers[] {
        { barrier }
    };

    command_buffer.pipelineBarrier(
        source_stage,
        destination_stage,
        { },
        nullptr,
        nullptr,
        image_barriers
    );

    image.layout = barrier.newLayout;
}

// =============================================================================
void destroy_image(ImageObject &image) {
    CONSOLE_TRACE("Destroying image {:#x}, view {:#x}, sampler {:#x}",
                   reinterpret_cast<uint64_t>(::VkImage(image.image)),
                   reinterpret_cast<uint64_t>(::VkImageView(image.view)),
                   reinterpret_cast<uint64_t>(::VkSampler(image.sampler)));

    LogicalDevice::native().destroy(image.image);
    LogicalDevice::native().destroy(image.view);
    LogicalDevice::native().destroy(image.sampler);
    VKAllocator::free(image.allocation);

    image = { };
}

// =============================================================================
void destroy_view(vk::ImageView &view) {
    CONSOLE_TRACE("Destroying image view {:#x}",
                   reinterpret_cast<uint64_t>(::VkImageView(view)));
    LogicalDevice::native().destroy(view);

    view = nullptr;
}

} // namespace ImageTools
} // namespace vkl