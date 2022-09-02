#ifndef VKLEARNIN_TOOLS_HPP
#define VKLEARNIN_TOOLS_HPP

#include "vklearnin/Allocator.hpp"

#include <vulkan/vulkan.hpp>

enum VmaMemoryUsage;

namespace ImageTools {

void init_image(const vk::Extent3D &extent, const vk::Format &format,
                const vk::ImageTiling &tiling,
                vk::Image &image_handle, const vk::ImageUsageFlags &usage,
                VmaAllocation &memory, VmaMemoryUsage memory_usage,
                uint32_t alloc_flags = 0u);

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