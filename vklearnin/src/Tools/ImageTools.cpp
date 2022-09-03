#include "vklearnin/common.hpp"
#include "vklearnin/Tools/ImageTools.hpp"

#include "vklearnin/Instance.hpp"

namespace ImageTools {

// =============================================================================
void init_image(const vk::Extent3D &extent, const vk::Format &format,
                const vk::ImageTiling &tiling, const uint32_t mip_levels,
                const vk::SampleCountFlagBits &sample_flag_bits,
                vk::Image &image_handle, const vk::ImageUsageFlags &usage,
                VmaAllocation &memory, VmaMemoryUsage memory_usage,
                uint32_t alloc_flags)
{
    CONSOLE_INFO("");

    vk::ImageCreateInfo image_info {
        .imageType = vk::ImageType::e2D,
        .format = format,
        .extent = extent,
        .mipLevels   = mip_levels,
        .arrayLayers = 1u,
        .samples     = sample_flag_bits,
        .tiling      = tiling,
        .usage       = usage,
        .sharingMode = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 0u,
        .pQueueFamilyIndices = nullptr,
        .initialLayout = vk::ImageLayout::eUndefined
    };

    ::VmaAllocationCreateInfo vma_info {
        .flags = alloc_flags,
        .usage = memory_usage,
        .requiredFlags = 0u,
        .preferredFlags = 0u,
        .memoryTypeBits = 0u,
        .pool = nullptr,
        .pUserData = nullptr,
        .priority = 1.0f
    };

    ::vmaCreateImage(
        Allocator::allocator(),
        &static_cast<::VkImageCreateInfo &>(image_info),
        &vma_info,
        &reinterpret_cast<::VkImage &>(image_handle),
        &memory,
        nullptr
    );

    CONSOLE_ERROR("Image object {}", fmt::ptr(&image_handle));

    // auto result = instance.logical_device().createImage(
    //     &image_info,
    //     nullptr,
    //     &image_handle
    // );

    // if(result != vk::Result::eSuccess) {
    //     CONSOLE_CRITICAL("Failed to create image handle");
    //     return;
    // }

    // auto memory_reqs =
    //     instance.logical_device().getImageMemoryRequirements(image_handle);

    // auto memory_type_index = BufferTools::find_memory_type(
    //     memory_reqs.memoryTypeBits,
    //     memory_flags,
    //     instance
    // );

    // vk::MemoryAllocateInfo memory_info {
    //     .allocationSize = memory_reqs.size,
    //     .memoryTypeIndex = memory_type_index,
    // };

    // device_memory = instance.logical_device().allocateMemory(memory_info);

    // if(result != vk::Result::eSuccess) {
    //     CONSOLE_ERROR("Could not allocate device memory for texture");
    //     return;
    // }

    // ::vkBindImageMemory(
    //     instance.logical_device(),
    //     image_handle,
    //     device_memory,
    //     0u
    // );
}

// =============================================================================
vk::ImageView init_view(const vk::Image &image, const vk::Format &color_format,
                        const uint32_t mip_levels,
                        const vk::ImageAspectFlags &aspect_flags,
                        const vk::Device &device)
{
    CONSOLE_INFO("");

    vk::ImageViewCreateInfo image_info {
        .image = image,
        .viewType = vk::ImageViewType::e2D,
        .format = color_format,
        .components = {
            .r = vk::ComponentSwizzle::eIdentity,
            .g = vk::ComponentSwizzle::eIdentity,
            .b = vk::ComponentSwizzle::eIdentity,
            .a = vk::ComponentSwizzle::eIdentity,
        },
        .subresourceRange {
            .aspectMask     = aspect_flags,
            .baseMipLevel   = 0u,
            .levelCount     = mip_levels,
            .baseArrayLayer = 0u,
            .layerCount     = 1u
        }
    };

    auto view = device.createImageView(image_info);

    return view;
}

// =============================================================================
void layout_transition(const vk::CommandBuffer &command_buffer,
                       const vk::Image &image_handle,
                       const uint32_t base_mip_level,
                       const uint32_t mip_levels,
                       const vk::ImageLayout &old_layout,
                       const vk::ImageLayout &new_layout)
{
    CONSOLE_INFO("");

    vk::ImageMemoryBarrier barrier {
        .oldLayout = old_layout,
        .newLayout = new_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image_handle,
        .subresourceRange {
            .aspectMask     = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel   = base_mip_level,
            .levelCount     = mip_levels,
            .baseArrayLayer = 0u,
            .layerCount     = 1u,
        }
    };

    vk::PipelineStageFlags source_stage      = vk::PipelineStageFlagBits::eNone;
    vk::PipelineStageFlags destination_stage = vk::PipelineStageFlagBits::eNone;

    if(old_layout == vk::ImageLayout::eUndefined) {
        if(new_layout == vk::ImageLayout::eTransferDstOptimal) {
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            source_stage      = vk::PipelineStageFlagBits::eTopOfPipe;
            destination_stage = vk::PipelineStageFlagBits::eTransfer;
        }
        else {
            CONSOLE_CRITICAL(
                "Unsupported image layout transition: from {} to {}",
                to_string(old_layout), to_string(new_layout)
            );
        }
    }
    else if(old_layout == vk::ImageLayout::eTransferDstOptimal) {
        if(new_layout == vk::ImageLayout::eShaderReadOnlyOptimal) {    
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            source_stage      = vk::PipelineStageFlagBits::eTransfer;
            destination_stage = vk::PipelineStageFlagBits::eFragmentShader;
        }
        else if(new_layout == vk::ImageLayout::eTransferSrcOptimal) {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

            source_stage      = vk::PipelineStageFlagBits::eTransfer;
            destination_stage = vk::PipelineStageFlagBits::eTransfer;
        }
        else {
            CONSOLE_CRITICAL(
                "Unsupported image layout transition: from {} to {}",
                to_string(old_layout), to_string(new_layout)
            );
        }
    }
    else if(old_layout == vk::ImageLayout::eTransferSrcOptimal) {
        if(new_layout == vk::ImageLayout::eShaderReadOnlyOptimal) {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            source_stage      = vk::PipelineStageFlagBits::eTransfer;
            destination_stage = vk::PipelineStageFlagBits::eFragmentShader;
        }
    }
    else {
        CONSOLE_CRITICAL(
            "Unsupported image layout transition: from {} to {}",
            to_string(old_layout), to_string(new_layout)
        );
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
}

bool has_stencil_component(const vk::Format &format) {
    return format == vk::Format::eS8Uint ||
           format == vk::Format::eD16UnormS8Uint ||
           format == vk::Format::eD24UnormS8Uint ||
           format == vk::Format::eD32SfloatS8Uint;
}

} // namespace vkt