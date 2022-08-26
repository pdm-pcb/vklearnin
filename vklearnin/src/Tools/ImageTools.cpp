#include "vklearnin/common.hpp"
#include "vklearnin/Tools/ImageTools.hpp"

namespace ImageTools {

// =============================================================================
void layout_transition(const ::VkCommandBuffer &command_buffer,
                       const ::VkImage &image_handle,
                       const ::VkFormat &image_format,
                       const ::VkImageAspectFlags &aspect_flags,
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
            .aspectMask     = aspect_flags,
            .baseMipLevel   = 0u,
            .levelCount     = 1u,
            .baseArrayLayer = 0u,
            .layerCount     = 1u,
        }
    };

    ::VkPipelineStageFlags source_stage      = 0u;
    ::VkPipelineStageFlags destination_stage = 0u;

    if(old_layout == ::VK_IMAGE_LAYOUT_UNDEFINED) {
        if(new_layout == ::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0u;
            barrier.dstAccessMask = ::VK_ACCESS_TRANSFER_WRITE_BIT;
            source_stage      = ::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destination_stage = ::VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
    }
    else if(old_layout == ::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        if(new_layout == ::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {    
            barrier.srcAccessMask = ::VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = ::VK_ACCESS_SHADER_READ_BIT;
            source_stage      = ::VK_PIPELINE_STAGE_TRANSFER_BIT;
            destination_stage = ::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
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

} // namespace vkt