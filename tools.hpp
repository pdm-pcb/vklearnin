#ifndef VKL_TOOLS_HPP
#define VKL_TOOLS_HPP

namespace tools {

void set_image_layout(::VkCommandBuffer cmd_buffer, ::VkImage image,
                      ::VkImageAspectFlags image_aspects,
                      ::VkImageLayout old_layout,
                      ::VkImageLayout new_layout);

} // namespace tools

#endif // VKL_TOOLS_HPP