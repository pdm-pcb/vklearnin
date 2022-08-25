#ifndef VKLEARNIN_TOOLS_HPP
#define VKLEARNIN_TOOLS_HPP

namespace ImageTools {

void set_image_layout(::VkCommandBuffer cmd_buffer, ::VkImage image,
                      ::VkImageAspectFlags image_aspects,
                      ::VkImageLayout old_layout,
                      ::VkImageLayout new_layout);

} // namespace tools

#endif // VKLEARNIN_TOOLS_HPP