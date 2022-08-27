#ifndef VKLEARNIN_TOOLS_HPP
#define VKLEARNIN_TOOLS_HPP

#include <vulkan/vulkan.h>

namespace ImageTools {

void init_image(const ::VkExtent3D extent, const ::VkFormat format,
                const ::VkImageTiling tiling, const ::VkImageUsageFlags usage,
                const ::VkMemoryPropertyFlags memory_flags,
                ::VkImage &image_handle, ::VkDeviceMemory &device_memory,
                const Instance &instance);

::VkImageView init_view(const ::VkImage &image, const ::VkFormat &color_format,
                        const ::VkImageAspectFlags &aspect_flags,
                        const ::VkDevice &device);

void layout_transition(const ::VkCommandBuffer &command_buffer,
                       const ::VkImage &image_handle,
                       const ::VkFormat &image_format,
                       const ::VkImageLayout &old_layout,
                       const ::VkImageLayout &new_layout);

bool has_stencil_component(const ::VkFormat format);

} // namespace tools

#endif // VKLEARNIN_TOOLS_HPP