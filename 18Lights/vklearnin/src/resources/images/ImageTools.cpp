#include "vklearnin/vklearnin.hpp"
#include "vklearnin/resources/images/ImageTools.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"
#include "vklearnin/system/devices/PhysicalDevice.hpp"
#include "vklearnin/system/devices/CmdBuffer.hpp"
#include "vklearnin/rendering/swapchain/Swapchain.hpp"

#include <stb/stb_image.h>

namespace vkl::ImageTools {

void allocate(ImageObject &image, vk::MemoryPropertyFlags const flags);

uint32_t find_memory_type(vk::MemoryPropertyFlags const flags,
                          vk::MemoryRequirements const &reqs);

void transition_layout(ImageObject &image,
                       vk::CommandBuffer const &command_buffer,
                       vk::ImageLayout const old_layout,
                       vk::ImageLayout const new_layout,
                       uint32_t const base_mip_level,
                       uint32_t const mip_levels,
                       uint32_t const base_array_layer,
                       uint32_t const array_layers);

// =============================================================================
void create(ImageObject &image,
            vk::ImageType const type,
            vk::ImageAspectFlags const aspect_flags,
            vk::SampleCountFlagBits const samples,
            vk::ImageUsageFlags const usage_flags,
            vk::MemoryPropertyFlags const memory_properties,
            vk::ImageCreateFlags const flags)
{
    if(image.handle) {
        CONSOLE_CRITICAL("Attempting to recreate an image object");
        return;
    }

    if(image.format == vk::Format::eUndefined) {
        CONSOLE_CRITICAL("Cannot create image with undefined format.");
        return;
    }

    vk::ImageCreateInfo const image_info {
        .flags         = flags,
        .imageType     = type,
        .format        = image.format,
        .extent        = image.extent,
        .mipLevels     = image.mip_levels,
        .arrayLayers   = image.array_layers,
        .samples       = samples,
        .tiling        = vk::ImageTiling::eOptimal,
        .usage         = usage_flags,
        .sharingMode   = vk::SharingMode::eExclusive,
        .initialLayout = vk::ImageLayout::eUndefined,
    };

    image.handle = LogicalDevice::native().createImage(image_info);
    image.aspect_flags = aspect_flags;

    CONSOLE_TRACE("Created image {:#x}",
                    reinterpret_cast<uint64_t>(::VkImage(image.handle)));

    allocate(image, memory_properties);
}

// =============================================================================
void destroy(ImageObject &image) {
    CONSOLE_TRACE("Destroying image {:#x}",
                   reinterpret_cast<uint64_t>(::VkImage(image.handle)));

    if(image.sampler) {
        destroy_sampler(image);
    }

    if(image.view) {
        destroy_view(image);
    }

    LogicalDevice::native().destroyImage(image.handle);
    LogicalDevice::native().freeMemory(image.memory);

    image.handle = nullptr;
    image.memory = nullptr;
}

// =============================================================================
void create_view(ImageObject &image,
                 vk::ImageViewType const view_type,
                 vk::ImageAspectFlags const &aspect_flags) {
    if(!image.handle) {
        CONSOLE_CRITICAL("Cannot create view for non-existant image.");
    }
    if(image.format == vk::Format::eUndefined) {
        CONSOLE_CRITICAL("Cannot create view for image with undefined format.");
    }

    vk::ImageViewCreateInfo const view_info {
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
            .baseMipLevel   = 0u,                // Starting mip level
            .levelCount     = image.mip_levels,  // Total mip levels
            .baseArrayLayer = 0u,                // Starting array layer
            .layerCount     = image.array_layers // Total array layers
        }
    };

    image.view = LogicalDevice::native().createImageView(view_info);
    CONSOLE_TRACE("Created image view {:#x}",
                    reinterpret_cast<uint64_t>(::VkImageView(image.view)));
}

// =============================================================================
void destroy_view(ImageObject &image) {
    CONSOLE_TRACE("Destroying image view {:#x}",
                   reinterpret_cast<uint64_t>(::VkImageView(image.view)));
    LogicalDevice::native().destroyImageView(image.view);
    image.view = nullptr;
}

// =============================================================================
void * image_from_file(ImageObject &image, std::string_view filepath) {
    auto const texture_path = ASSET_PATH / filepath.data();
    std::string const path = texture_path.string();

    int width    = 0;
    int height   = 0;
    int channels = 0;

    ::stbi_uc* data = ::stbi_load(
        path.c_str(),
        &width, &height, &channels,
        ::STBI_rgb_alpha
    );

    if(data == nullptr) {
        CONSOLE_CRITICAL(
            "Failed to load image '{}'\n\t"
            "{}x{} @ {}bpc\n\t"
            "Error: '{}'",
            path, width, height, channels,
            ::stbi_failure_reason()
        );
    }
    else {
        CONSOLE_TRACE("Loaded image {}", filepath);

        image.extent = vk::Extent3D {
            .width  = static_cast<uint32_t>(width),
            .height = static_cast<uint32_t>(height),
            .depth  = 1u
        };

        channels = static_cast<uint32_t>(::STBI_rgb_alpha);
        image.size = image.extent.width * image.extent.height * channels;
        image.format = vk::Format::eR8G8B8A8Unorm;
    }

    return data;
}

// =============================================================================
void * cubemap_from_files(ImageObject &image,
                          std::array<std::string_view, 6> const &filepaths)
{
    struct ImageData {
        ::stbi_uc *data = nullptr;

        int width    = 0;
        int height   = 0;
        int channels = 0;
    };

    std::vector<ImageData> image_data;
    image_data.resize(filepaths.size());
    auto current_image = image_data.begin();

    for(auto const filepath : filepaths) {
        auto const texture_path = ASSET_PATH / filepath.data();
        const std::string path = texture_path.string();

        current_image->data = ::stbi_load(
            path.c_str(),
            &(current_image->width),
            &(current_image->height),
            &(current_image->channels),
            ::STBI_rgb_alpha
        );

        if(current_image->data == nullptr) {
            CONSOLE_CRITICAL(
                "Failed to load image '{}'\n\t"
                "{}x{} @ {}bpc\n\t"
                "Error: '{}'",
                path,
                current_image->width,
                current_image->height,
                current_image->channels,
                ::stbi_failure_reason()
            );
            return nullptr;
        }

        current_image->channels = static_cast<uint32_t>(::STBI_rgb_alpha);

        CONSOLE_TRACE("Loaded image {}", filepath);
        ++current_image;
    }

    current_image = std::next(image_data.begin());
    while(current_image != image_data.end()) {
        if(current_image->width != image_data.begin()->width ||
           current_image->height != image_data.begin()->height)
        {
            CONSOLE_ERROR("Cubemap images must have identical dimensions");
            return nullptr;
        }
        ++current_image;
    }

    image.array_layers = static_cast<uint32_t>(filepaths.size());

    image.extent = vk::Extent3D {
        .width  = static_cast<uint32_t>(image_data.begin()->width),
        .height = static_cast<uint32_t>(image_data.begin()->height),
        .depth  = 1u
    };

    image.size = image.extent.width *
                 image.extent.height *
                 image_data.begin()->channels *
                 filepaths.size();

    image.format = vk::Format::eR8G8B8A8Unorm;

    char *consolidated_image = new char[image.size];
    char *offset = consolidated_image;
    size_t const layer_size = image.extent.width *
                              image.extent.height *
                              image_data.begin()->channels;

    for(auto const &image_struct : image_data) {
        memcpy(offset, image_struct.data, layer_size);
        offset += layer_size;
    }

    for(auto const &image_struct : image_data) {
        ::stbi_image_free(image_struct.data);
    }

    return consolidated_image;
}

// =============================================================================
void free_image_data(void *data) {
    ::stbi_image_free(data);
}

// =============================================================================
void free_cubemap_data(void *data) {
    auto *allocated_data = static_cast<char *>(data);
    delete[] allocated_data;
}

// =============================================================================
void host_to_device(ImageObject &dst, void const * const data) {
    vk::BufferImageCopy const copy_region {
        .bufferOffset = 0u,
        .bufferRowLength = 0u,
        .bufferImageHeight = 0u,
        .imageSubresource {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .mipLevel = 0u,
            .baseArrayLayer = 0u,
            .layerCount = dst.array_layers,
        },
        .imageOffset {
            .x = 0,
            .y = 0,
            .z = 0
        },
        .imageExtent = dst.extent
    };

    auto staging_buffer = BufferTools::stage_data(dst.size, data);

    CmdBuffer cmd_buffer;
    cmd_buffer.allocate(LogicalDevice::transient_pool().native());
    cmd_buffer.begin_one_time_submit();

        transition_layout(
            dst,
            cmd_buffer.native(),
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eTransferDstOptimal,
            0u,
            dst.mip_levels,
            0u,
            dst.array_layers
        );

        cmd_buffer.native().copyBufferToImage(
            staging_buffer.handle,
            dst.handle,
            dst.layout,
            copy_region
        );

    cmd_buffer.end_recording();
    cmd_buffer.submit_and_wait_on_device();
    cmd_buffer.free();

    BufferTools::destroy(staging_buffer);

    CONSOLE_TRACE("Copied {} bytes from staging buffer", dst.size);
}

// =============================================================================
void create_sampler(ImageObject &image,
                    vk::Filter const min_filter,
                    vk::Filter const mag_filter,
                    vk::SamplerMipmapMode const mip_filter,
                    vk::SamplerAddressMode const mode_u,
                    vk::SamplerAddressMode const mode_v)
{
    vk::SamplerCreateInfo const sampler_info {
        .magFilter        = mag_filter,
        .minFilter        = min_filter,
        .mipmapMode       = mip_filter,
        .addressModeU     = mode_u,
        .addressModeV     = mode_v,
        .mipLodBias       = 0.0f,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy    = RenderConfig::anisotropy,
        .compareEnable    = VK_FALSE,
        .compareOp        = vk::CompareOp::eAlways,
        .minLod           = 0.0f,
        .maxLod           = VK_LOD_CLAMP_NONE,
        .borderColor      = vk::BorderColor::eIntOpaqueWhite,
        .unnormalizedCoordinates = VK_FALSE
    };

    image.sampler = LogicalDevice::native().createSampler(sampler_info);

    CONSOLE_TRACE(
        "\nCreated image sampler {:#x}"
        "\n\tMag Filter: {}"
        "\n\tMin Filter: {}"
        "\n\tMip Mode:   {}"
        "\n\tAddress U:  {}"
        "\n\tAddress V:  {}"
        "\n\tAnisotropy: {}",
        reinterpret_cast<uint64_t>(::VkSampler(image.sampler)),
        to_string(sampler_info.magFilter),
        to_string(sampler_info.minFilter),
        to_string(sampler_info.mipmapMode),
        to_string(sampler_info.addressModeU),
        to_string(sampler_info.addressModeV),
        sampler_info.maxAnisotropy
    );
}

// =============================================================================
void destroy_sampler(ImageObject &image) {
    CONSOLE_TRACE("Destroying image sampler {:#x}",
                   reinterpret_cast<uint64_t>(::VkSampler(image.sampler)));
    LogicalDevice::native().destroySampler(image.sampler);
    image.sampler = nullptr;
}

// =============================================================================
void generate_mipmap(ImageObject &image, const vk::Filter filter) {
    auto const format_props =
        PhysicalDevice::native().getFormatProperties(image.format);

    // TODO: Is there any crossover here between vk::Filter and FormatFeature?
    //       While vk::Filter supports nearest, FormatFeature appears to not.
    //       Likewise, FormatFeature supports "MinMax" while Filter does not.
    if(!(format_props.optimalTilingFeatures &
         vk::FormatFeatureFlagBits::eSampledImageFilterLinear))
    {
        CONSOLE_CRITICAL(
            "Image format {} does not support linear data blitting.",
            to_string(image.format)
        );
        return;
    }

    CmdBuffer cmd_buffer;
    cmd_buffer.allocate(LogicalDevice::transient_pool().native());
    cmd_buffer.begin_one_time_submit();

    for(uint32_t layer = 0u; layer < image.array_layers; ++layer) {
        // CONSOLE_TRACE("Processing array layer {}", layer);

        int32_t mip_width  = static_cast<int32_t>(image.extent.width);
        int32_t mip_height = static_cast<int32_t>(image.extent.height);

        for(uint32_t mip = 1u; mip < image.mip_levels; ++mip) {
            // CONSOLE_TRACE("Generating mip level {}", mip);
            transition_layout(
                image,
                cmd_buffer.native(),
                vk::ImageLayout::eTransferDstOptimal,
                vk::ImageLayout::eTransferSrcOptimal,
                mip - 1u,
                1u,
                layer,
                1u
            );

            vk::ImageBlit const blit {
                .srcSubresource {
                    .aspectMask     = vk::ImageAspectFlagBits::eColor,
                    .mipLevel       = mip - 1,
                    .baseArrayLayer = layer,
                    .layerCount     = 1u,
                },
                .srcOffsets = std::array<vk::Offset3D, 2> {
                    vk::Offset3D { 0, 0, 0 },
                    vk::Offset3D { mip_width, mip_height, 1 }
                },
                .dstSubresource {
                    .aspectMask     = vk::ImageAspectFlagBits::eColor,
                    .mipLevel       = mip,
                    .baseArrayLayer = layer,
                    .layerCount     = 1u,
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

            cmd_buffer.native().blitImage(
                image.handle,
                vk::ImageLayout::eTransferSrcOptimal,
                image.handle,
                vk::ImageLayout::eTransferDstOptimal,
                { blit },
                filter
            );

            transition_layout(
                image,
                cmd_buffer.native(),
                vk::ImageLayout::eTransferSrcOptimal,
                vk::ImageLayout::eShaderReadOnlyOptimal,
                mip - 1u,
                1u,
                layer,
                1u
            );

            if(mip_width  > 1) { mip_width  /= 2; }
            if(mip_height > 1) { mip_height /= 2; }
        }

        transition_layout(
            image,
            cmd_buffer.native(),
            vk::ImageLayout::eTransferDstOptimal,
            vk::ImageLayout::eShaderReadOnlyOptimal,
            image.mip_levels - 1u,
            1u,
            layer,
            1u
        );
    }

    cmd_buffer.end_recording();
    cmd_buffer.submit_and_wait_on_device();
    cmd_buffer.free();
}

// =============================================================================
void allocate(ImageObject &image, vk::MemoryPropertyFlags const flags) {
    vk::MemoryRequirements mem_reqs { };
    LogicalDevice::native().getImageMemoryRequirements(
        image.handle,
        &mem_reqs
    );

    auto const type_index = find_memory_type(flags, mem_reqs);

    vk::MemoryAllocateInfo const alloc_info {
        .allocationSize  = mem_reqs.size,
        .memoryTypeIndex = type_index,
    };

    image.memory = LogicalDevice::native().allocateMemory(alloc_info);

    CONSOLE_TRACE(
        "\n\tAllocated {} bytes : {:#x}"
        "\n\tFor image {:#x}",
        mem_reqs.size,
        reinterpret_cast<uint64_t>(VkDeviceMemory(image.memory)),
        reinterpret_cast<uint64_t>(VkImage(image.handle))
    );

    LogicalDevice::native().bindImageMemory(
        image.handle,
        image.memory,
        0u
    );
}

// =============================================================================
uint32_t find_memory_type(vk::MemoryPropertyFlags const flags,
                          vk::MemoryRequirements const &reqs)
{
    auto const& memory_properties = PhysicalDevice::memory_props();
    auto const type_count = memory_properties.memoryTypeCount;

    for(uint32_t type_index = 0u; type_index < type_count; ++type_index) {
        if((reqs.memoryTypeBits & (1u << type_index)) != 0u) {
            auto const& props = memory_properties.memoryTypes[type_index];
            if(props.propertyFlags & flags) {
                return type_index;
            }
        }
    }

    CONSOLE_CRITICAL("Could not find memory to match buffer requirements.");
    return std::numeric_limits<uint32_t>::max();
}

// =============================================================================
void transition_layout(ImageObject &image,
                       vk::CommandBuffer const &command_buffer,
                       vk::ImageLayout const old_layout,
                       vk::ImageLayout const new_layout,
                       uint32_t const base_mip_level,
                       uint32_t const mip_levels,
                       uint32_t const base_array_layer,
                       uint32_t const array_layers)
{
    vk::ImageMemoryBarrier barrier {
        .oldLayout = old_layout,
        .newLayout = new_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image.handle,
        .subresourceRange {
            .aspectMask     = image.aspect_flags,
            .baseMipLevel   = base_mip_level,
            .levelCount     = mip_levels,
            .baseArrayLayer = base_array_layer,
            .layerCount     = array_layers,
        }
    };

    // CONSOLE_TRACE(
    //     "Image {:#x}, mip {} ({}), layer {} ({}): '{}' -> '{}'",
    //     reinterpret_cast<uint64_t>(VkImage(image.handle)),
    //     base_mip_level, mip_levels,
    //     base_array_layer, array_layers,
    //     to_string(barrier.oldLayout),
    //     to_string(barrier.newLayout)
    // );

    vk::PipelineStageFlags src_stage = vk::PipelineStageFlagBits::eNone;
    vk::PipelineStageFlags dst_stage = vk::PipelineStageFlagBits::eNone;

    if(barrier.oldLayout == vk::ImageLayout::eUndefined) {
        if(new_layout == vk::ImageLayout::eTransferDstOptimal) {
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            src_stage = vk::PipelineStageFlagBits::eTopOfPipe;
            dst_stage = vk::PipelineStageFlagBits::eTransfer;
        }
        else {
            CONSOLE_CRITICAL("Unsupported image layout transition");
            return;
        }
    }
    else if(barrier.oldLayout == vk::ImageLayout::eTransferDstOptimal) {
        if(barrier.newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            src_stage      = vk::PipelineStageFlagBits::eTransfer;
            dst_stage = vk::PipelineStageFlagBits::eFragmentShader;
        }
        else if(barrier.newLayout == vk::ImageLayout::eTransferSrcOptimal) {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

            src_stage      = vk::PipelineStageFlagBits::eTransfer;
            dst_stage = vk::PipelineStageFlagBits::eTransfer;
        }
        else {
            CONSOLE_CRITICAL("Unsupported image layout transition");
            return;
        }
    }
    else if(barrier.oldLayout == vk::ImageLayout::eTransferSrcOptimal) {
        if(barrier.newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            src_stage      = vk::PipelineStageFlagBits::eTransfer;
            dst_stage = vk::PipelineStageFlagBits::eFragmentShader;
        }
        else {
            CONSOLE_CRITICAL("Unsupported image layout transition");
            return;
        }
    }
    else {
        CONSOLE_CRITICAL("Unsupported image layout transition");
        return;
    }

    command_buffer.pipelineBarrier(
        src_stage,
        dst_stage,
        { },
        nullptr,
        nullptr,
        { barrier }
    );

    image.layout = barrier.newLayout;
}

} // namespace vkl::ImageTools