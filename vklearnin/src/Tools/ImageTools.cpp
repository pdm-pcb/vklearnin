#include "vklearnin/common.hpp"
#include "vklearnin/Tools/ImageTools.hpp"

#include "vklearnin/Instance.hpp"

namespace ImageTools {

// =============================================================================
void init_image(const ::VkExtent3D extent, const ::VkFormat format,
                const ::VkImageTiling tiling, const ::VkImageUsageFlags usage,
                const ::VkMemoryPropertyFlags memory_flags,
                ::VkImage &image_handle, ::VkDeviceMemory &device_memory,
                const Instance &instance)
{
    CONSOLE_INFO("");

    ::VkImageCreateInfo image_info {
        .sType = ::VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0u,
        .imageType = ::VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = extent,
        .mipLevels   = 1u,
        .arrayLayers = 1u,
        .samples     = ::VK_SAMPLE_COUNT_1_BIT,
        .tiling      = tiling,
        .usage       = usage,
        .sharingMode = ::VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0u,
        .pQueueFamilyIndices = nullptr,
        .initialLayout = ::VK_IMAGE_LAYOUT_UNDEFINED
    };

    auto result = ::vkCreateImage(
        instance.logical_device(),
        &image_info,
        nullptr,
        &image_handle
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_CRITICAL("Failed to create image handle");
        return;
    }

    ::VkMemoryRequirements memory_reqs { };
    ::vkGetImageMemoryRequirements(
        instance.logical_device(),
        image_handle,
        &memory_reqs
    );

    auto memory_type_index = BufferTools::find_memory_type(
        memory_reqs.memoryTypeBits,
        memory_flags,
        instance
    );

    ::VkMemoryAllocateInfo memory_info {
        .sType = ::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = memory_reqs.size,
        .memoryTypeIndex = memory_type_index,
    };

    result = ::vkAllocateMemory(
        instance.logical_device(),
        &memory_info,
        nullptr,
        &device_memory
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_ERROR("Could not allocate device memory for texture");
        return;
    }

    ::vkBindImageMemory(
        instance.logical_device(),
        image_handle,
        device_memory,
        0u
    );
}

// =============================================================================
::VkImageView init_view(const ::VkImage &image, const ::VkFormat &color_format,
                        const ::VkImageAspectFlags &aspect_flags,
                        const ::VkDevice &device)
{
    CONSOLE_INFO("");

    ::VkImageViewCreateInfo image_info {
        .sType = ::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0u,
        .image = image,
        .viewType = ::VK_IMAGE_VIEW_TYPE_2D,
        .format = color_format,
        .components = {
            .r = ::VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = ::VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = ::VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = ::VK_COMPONENT_SWIZZLE_IDENTITY,
        },
        .subresourceRange {
            .aspectMask     = aspect_flags,
            .baseMipLevel   = 0u,
            .levelCount     = 1u,
            .baseArrayLayer = 0u,
            .layerCount     = 1u
        }
    };

    ::VkImageView view; 
    auto result = ::vkCreateImageView(
        device,
        &image_info,
        nullptr,
        &view
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_ERROR("Failed to create image view.");
    }

    return view;
}

// =============================================================================
void layout_transition(const ::VkCommandBuffer &command_buffer,
                       const ::VkImage &image_handle,
                       const ::VkFormat &image_format,
                       const ::VkImageLayout &old_layout,
                       const ::VkImageLayout &new_layout)
{
    CONSOLE_INFO("");

    ::VkImageMemoryBarrier barrier {
        .sType = ::VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = nullptr,
        .srcAccessMask = 0u,
        .dstAccessMask = 0u,
        .oldLayout = old_layout,
        .newLayout = new_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image_handle,
        .subresourceRange {
            .aspectMask     = 0u,
            .baseMipLevel   = 0u,
            .levelCount     = 1u,
            .baseArrayLayer = 0u,
            .layerCount     = 1u,
        }
    };

    ::VkPipelineStageFlags source_stage      = ::VK_PIPELINE_STAGE_NONE;
    ::VkPipelineStageFlags destination_stage = ::VK_PIPELINE_STAGE_NONE;

    if(old_layout == ::VK_IMAGE_LAYOUT_UNDEFINED) {
        if(new_layout == ::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0u;
            barrier.dstAccessMask = ::VK_ACCESS_TRANSFER_WRITE_BIT;

            source_stage      = ::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destination_stage = ::VK_PIPELINE_STAGE_TRANSFER_BIT;

            barrier.subresourceRange.aspectMask = ::VK_IMAGE_ASPECT_COLOR_BIT;
        }
        else {
            CONSOLE_CRITICAL("Unsupported new image layout with old layout "
                             "set to undefined");
        }
    }
    else if(old_layout == ::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        if(new_layout == ::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {    
            barrier.srcAccessMask = ::VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = ::VK_ACCESS_SHADER_READ_BIT;

            source_stage      = ::VK_PIPELINE_STAGE_TRANSFER_BIT;
            destination_stage = ::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

            barrier.subresourceRange.aspectMask = ::VK_IMAGE_ASPECT_COLOR_BIT;
        }
        else {
            CONSOLE_CRITICAL("Unsupported new image layout with old layout "
                             "set to transfer destination optimal");
        }
    }
    else {
        CONSOLE_CRITICAL("Unsupported old image layout");
    }

    ::VkImageMemoryBarrier image_barriers[] {
        { barrier }
    };

    ::vkCmdPipelineBarrier(
        command_buffer,
        source_stage,
        destination_stage,
        0u,
        0u, nullptr,
        0u, nullptr,
        std::size(image_barriers), image_barriers
    );
}

bool has_stencil_component(const ::VkFormat format) {
    return format == VK_FORMAT_S8_UINT ||
           format == VK_FORMAT_D16_UNORM_S8_UINT ||
           format == VK_FORMAT_D24_UNORM_S8_UINT ||
           format == VK_FORMAT_D32_SFLOAT_S8_UINT;
}

} // namespace vkt