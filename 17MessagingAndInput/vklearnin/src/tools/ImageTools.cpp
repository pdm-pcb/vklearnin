#include "vklearnin/vklearnin.hpp"
#include "vklearnin/tools/ImageTools.hpp"

#include "vklearnin/rendering/devices/PhysicalDevice.hpp"
#include "vklearnin/rendering/devices/LogicalDevice.hpp"
#include "vklearnin/tools/VKAllocator.hpp"

#include <stb/stb_image.h>

namespace vkl {
namespace ImageTools {

// =============================================================================
ImageObject load_from_file(std::string_view filepath, const bool flip_vertical) {
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

    result = ImageTools::create_image(
        vk::Extent3D {
            .width  = static_cast<uint32_t>(width),
            .height = static_cast<uint32_t>(height),
            .depth  = 1u
        },
        static_cast<uint8_t>(channels),
        vk::Format::eR8G8B8A8Unorm,
        vk::ImageAspectFlagBits::eColor,
        vk::ImageTiling::eOptimal,
        1u,
        vk::SampleCountFlagBits::e1,
        (vk::ImageUsageFlagBits::eTransferDst |
         vk::ImageUsageFlagBits::eSampled),
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        "tex from file"
    );

    result.width = static_cast<uint32_t>(width);
    result.height = static_cast<uint32_t>(height);
    result.channels = static_cast<uint32_t>(channels);

    result.size = result.width * result.height * ::STBI_rgb_alpha;
    auto staging_buffer =
        BufferTools::stage_data(result.size, image_data);
    ImageTools::move_to_device(
        staging_buffer,
        result,
        { result.width, result.height, 1u }
    );

    ::stbi_image_free(image_data);
    BufferTools::destroy_buffer(staging_buffer);

    result.sampler = ImageTools::create_sampler(
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
ImageObject create_image(const vk::Extent3D &extent,
                         const uint8_t channels,
                         const vk::Format &color_format,
                         const vk::ImageAspectFlags &image_aspect,
                         const vk::ImageTiling &tiling,
                         const uint32_t mip_levels,
                         const vk::SampleCountFlagBits &sample_count,
                         const vk::ImageUsageFlags &usage,
                         const vk::MemoryPropertyFlags memory_properties,
                         std::string_view image_name)
{
    vk::ImageCreateInfo image_info {
        .imageType   = vk::ImageType::e2D,
        .format      = color_format,
        .extent      = extent,
        .mipLevels   = mip_levels,
        .arrayLayers = 1u,
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
    }

    auto alloc = VKAllocator::allocate(result.value, memory_properties, image_name);
    auto view = create_view(result.value, color_format, image_aspect);

    return {
        .image  = result.value,
        .view   = view,
        .format = color_format,
        .layout = vk::ImageLayout::eUndefined,
        .width  = extent.width,
        .height = extent.height,
        .channels = channels,
        .allocation = alloc,
    };
}

// =============================================================================
void destroy_image(ImageObject &image) {
    CONSOLE_TRACE("Destroying image view {:#x}, sampler {:#x}",
                   reinterpret_cast<uint64_t>(::VkImageView(image.view)),
                   reinterpret_cast<uint64_t>(::VkSampler(image.sampler)));

    LogicalDevice::native().destroy(image.image);
    LogicalDevice::native().destroy(image.view);
    
    VKAllocator::free(image.allocation);

    LogicalDevice::native().destroy(image.sampler);


    // // I want these here... but when they're not, they help catch double
    // // deletions. =)
    // image.image   = { nullptr };
    // image.view    = { nullptr };
    // image.format  = { };
    // image.layout  = { };
    // image.sampler = { nullptr };

    // image.width    = 0u;
    // image.height   = 0u;
    // image.channels = 0u;

    // image.allocation = { };
}

// =============================================================================
vk::ImageView create_view(const vk::Image &image,
                          const vk::Format &color_format,
                          const vk::ImageAspectFlags &image_aspect)
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
            .aspectMask     = image_aspect,
            .baseMipLevel   = 0u,
            .levelCount     = 1u,
            .baseArrayLayer = 0u,
            .layerCount     = 1u
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
    return view;
}

// =============================================================================
void destroy_view(const vk::ImageView &view)
{
    CONSOLE_TRACE("Destroying image view {:#x}",
                   reinterpret_cast<uint64_t>(::VkImageView(view)));
    LogicalDevice::native().destroy(view);
}

// =============================================================================
vk::Sampler create_sampler(const vk::Filter min_filter,
                           const vk::Filter mag_filter,
                           const vk::SamplerMipmapMode mip_filter,
                           const vk::SamplerAddressMode address_mode_u,
                           const vk::SamplerAddressMode address_mode_v,
                           const vk::SamplerAddressMode address_mode_w)
{
    vk::PhysicalDeviceProperties gpu_props { };
    PhysicalDevice::native().getProperties(&gpu_props);

    vk::SamplerCreateInfo sampler_info {
        .magFilter        = min_filter,
        .minFilter        = mag_filter,
        .mipmapMode       = mip_filter,
        .addressModeU     = address_mode_u,
        .addressModeV     = address_mode_v,
        .addressModeW     = address_mode_w,
        .mipLodBias       = 0.0f,
        .anisotropyEnable = true,
        .maxAnisotropy    = gpu_props.limits.maxSamplerAnisotropy,
        .compareEnable    = false,
        .compareOp        = vk::CompareOp::eAlways,
        .minLod           = 1.0f,
        .maxLod           = 1.0f,
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

    return result.value;
}

// =============================================================================
void transition_layout(ImageObject &image, const vk::ImageLayout new_layout) {
    auto command_buffer = BufferTools::begin_oneshot_cmd_buffer();

    vk::ImageMemoryBarrier barrier {
        .oldLayout = image.layout,
        .newLayout = new_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image.image,
        .subresourceRange {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0u,
            .levelCount = 1u,
            .baseArrayLayer = 0u,
            .layerCount = 1u,
        }
    };

    vk::PipelineStageFlags source_flags;
    vk::PipelineStageFlags dest_flags;

    CONSOLE_TRACE(
        "Image {:#x} layout {} -> {}",
        reinterpret_cast<uint64_t>(VkImage(image.image)),
        to_string(image.layout),
        to_string(new_layout)
    );
    if(image.layout == vk::ImageLayout::eUndefined)
    {
        if(new_layout == vk::ImageLayout::eTransferDstOptimal) {
            barrier.srcAccessMask = vk::AccessFlagBits::eNone;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            source_flags = vk::PipelineStageFlagBits::eTopOfPipe;
            dest_flags = vk::PipelineStageFlagBits::eTransfer;
        }
        else if(new_layout == vk::ImageLayout::eDepthAttachmentOptimal) {
            barrier.srcAccessMask = vk::AccessFlagBits::eNone;
            barrier.dstAccessMask =
                (vk::AccessFlagBits::eDepthStencilAttachmentRead |
                 vk::AccessFlagBits::eDepthStencilAttachmentWrite);

            source_flags = vk::PipelineStageFlagBits::eTopOfPipe;
            dest_flags = vk::PipelineStageFlagBits::eEarlyFragmentTests;
        }
    }
    else if(image.layout == vk::ImageLayout::eTransferDstOptimal &&
            new_layout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        source_flags = vk::PipelineStageFlagBits::eTransfer;
        dest_flags = vk::PipelineStageFlagBits::eFragmentShader;
    } else {
        CONSOLE_CRITICAL("Unsupported image layout transition");
    }

    command_buffer.pipelineBarrier(
        source_flags,
        dest_flags,
        { }, { }, { },
        { barrier }
    );

    BufferTools::end_oneshot_cmd_buffer(command_buffer);

    image.layout = new_layout;
}

// =============================================================================
void move_to_device(const BufferObject &source, ImageObject &dest,
                    const vk::Extent3D &extent)
{
    auto command_buffer = BufferTools::begin_oneshot_cmd_buffer();

    vk::BufferImageCopy copy_region {
        .bufferOffset = 0u,
        .bufferRowLength = 0u,
        .bufferImageHeight = 0u,
        .imageSubresource {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .mipLevel = 0u,
            .baseArrayLayer = 0u,
            .layerCount = 1u,
        },
        .imageOffset {
            .x = 0,
            .y = 0,
            .z = 0
        },
        .imageExtent = extent
    };

    transition_layout(dest, vk::ImageLayout::eTransferDstOptimal);
        command_buffer.copyBufferToImage(
            source.buffer,
            dest.image,
            dest.layout,
            copy_region
        );    
        BufferTools::end_oneshot_cmd_buffer(command_buffer);
    transition_layout(dest, vk::ImageLayout::eShaderReadOnlyOptimal);
}

} // namespace ImageTools
} // namespace vkl