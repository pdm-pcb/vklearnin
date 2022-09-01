#ifndef VKLEARNIN_TOOLS_HPP
#define VKLEARNIN_TOOLS_HPP

#include <vulkan/vulkan.hpp>

namespace ImageTools {

void init_image(const vk::Extent3D &extent, const vk::Format &format,
                const vk::ImageTiling &tiling, const vk::ImageUsageFlags &usage,
                const vk::MemoryPropertyFlags &memory_flags,
                vk::Image &image_handle, vk::DeviceMemory &device_memory,
                const Instance &instance);

vk::ImageView init_view(const vk::Image &image, const vk::Format &color_format,
                        const vk::ImageAspectFlags &aspect_flags,
                        const vk::Device &device);

void layout_transition(const vk::CommandBuffer &command_buffer,
                       const vk::Image &image_handle,
                       const vk::Format &image_format,
                       const vk::ImageLayout &old_layout,
                       const vk::ImageLayout &new_layout);

bool has_stencil_component(const vk::Format &format);

} // namespace tools

#endif // VKLEARNIN_TOOLS_HPP