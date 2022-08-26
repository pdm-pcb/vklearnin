#ifndef VKLEARNIN_TOOLS_HPP
#define VKLEARNIN_TOOLS_HPP

#include <vulkan/vulkan.h>

namespace ImageTools {

void layout_transition(const ::VkCommandBuffer &command_buffer,
                       const ::VkImage &image_handle,
                       const ::VkFormat &image_format,
                       const ::VkImageAspectFlags &aspect_flags,
                       const ::VkImageLayout &old_layout,
                       const ::VkImageLayout &new_layout);

::VkImageView init_view(const ::VkImage image, const ::VkFormat &color_format,
                        const ::VkDevice &device);

} // namespace tools

#endif // VKLEARNIN_TOOLS_HPP