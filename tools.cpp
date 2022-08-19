#include "common.hpp"
#include "tools.hpp"

namespace tools {

void set_image_layout(::VkCommandBuffer cmd_buffer,
                      ::VkImage image,
                      ::VkImageAspectFlags image_aspects,
                      ::VkImageLayout old_layout,
                      ::VkImageLayout new_layout)
{
    CONSOLE_INFO("");
    ::VkImageMemoryBarrier barrier { };
    barrier.sType = ::VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.pNext = nullptr;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = image_aspects;
    barrier.subresourceRange.baseMipLevel = 0u;
    barrier.subresourceRange.levelCount   = 1u;
    barrier.subresourceRange.layerCount   = 1u;

    switch(old_layout) {
        case ::VK_IMAGE_LAYOUT_UNDEFINED:
            CONSOLE_WARN("Old image layout undefined");
            break;

        case ::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            barrier.srcAccessMask = ::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;
        
        case ::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            barrier.srcAccessMask =
                ::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;
        
        case ::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            barrier.srcAccessMask = ::VK_ACCESS_SHADER_READ_BIT;
            break;
        
        case ::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            barrier.srcAccessMask = ::VK_ACCESS_TRANSFER_READ_BIT;
            break;

        case ::VK_IMAGE_LAYOUT_PREINITIALIZED:
            barrier.srcAccessMask =
                ::VK_ACCESS_HOST_WRITE_BIT | ::VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        default:
            CONSOLE_CRITICAL("Unknown old image layout {}", old_layout);
            break;
    }

    switch(new_layout) {
        case ::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            barrier.srcAccessMask = ::VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = ::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case ::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            barrier.srcAccessMask =
                ::VK_ACCESS_HOST_WRITE_BIT | ::VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = ::VK_ACCESS_SHADER_READ_BIT;
            break;
        
        case ::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            barrier.srcAccessMask |= ::VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask  = ::VK_ACCESS_TRANSFER_READ_BIT;
            break;
        
        case ::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            barrier.dstAccessMask = ::VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case ::VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
            barrier.dstAccessMask |=
                ::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;

        case ::VK_IMAGE_LAYOUT_UNDEFINED:
            CONSOLE_CRITICAL("New image layout cannot be undefined");
            break;

        case ::VK_IMAGE_LAYOUT_PREINITIALIZED:
            CONSOLE_CRITICAL("New image layout cannot be preinitialized");
            break;

        default:
            CONSOLE_CRITICAL("Unknown new image layout {}", old_layout);
            break;
    }

    ::vkCmdPipelineBarrier(
        cmd_buffer,
        ::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        ::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        0, 0, nullptr, 0, nullptr, 1,
        &barrier
    );
}

} // namespace vkt