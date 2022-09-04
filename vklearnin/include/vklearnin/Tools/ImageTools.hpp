#ifndef VKLEARNIN_TOOLS_HPP
#define VKLEARNIN_TOOLS_HPP

#include "vklearnin/pch.hpp"
#include "vklearnin/Tools/Allocator.hpp"

namespace ImageTools {

void init_image(const vk::Extent3D &extent, const vk::Format &format,
                const vk::ImageTiling &tiling, const uint32_t mip_levels,
                const vk::SampleCountFlagBits &sample_flag_bits,
                vk::Image &image, const vk::ImageUsageFlags &usage,
                VmaAllocation &memory, VmaMemoryUsage memory_usage,
                uint32_t alloc_flags = 0u, const char *alloc_name = nullptr);

void destroy_image(vk::Image &image, ::VmaAllocation &memory);

vk::ImageView init_view(const vk::Image &image, const vk::Format &color_format,
                        const uint32_t mip_levels,
                        const vk::ImageAspectFlags &aspect_flags,
                        const vk::Device &device);

void layout_transition(const vk::CommandBuffer &command_buffer,
                       const vk::Image &image_handle,
                       const uint32_t base_mip_level,
                       const uint32_t mip_levels,
                       const vk::ImageLayout &old_layout,
                       const vk::ImageLayout &new_layout);

bool has_stencil_component(const vk::Format &format);

} // namespace tools

#endif // VKLEARNIN_TOOLS_HPP