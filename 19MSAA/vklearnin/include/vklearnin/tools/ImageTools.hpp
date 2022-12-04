#ifndef VKLEARNIN_TOOLS_IMAGETOOLS_HPP
#define VKLEARNIN_TOOLS_IMAGETOOLS_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/shaders/ImageObject.hpp"

namespace vkl {

struct BufferObject;

namespace ImageTools {

ImageObject load_from_file(std::string_view filepath,
                           const bool flip_vertical = false);

ImageObject create_image(const vk::Extent3D &extent,
                         const uint8_t channels,
                         const vk::Format &color_format,
                         const vk::ImageAspectFlags &image_aspect,
                         const vk::ImageTiling &tiling,
                         const uint32_t mip_levels,
                         const vk::SampleCountFlagBits &sample_count,
                         const vk::ImageUsageFlags &usage,
                         const vk::MemoryPropertyFlags memory_properties,
                         std::string_view image_name);

void create_view(ImageObject &image, const vk::Format &color_format,
                 const vk::ImageAspectFlags &image_aspect);

void create_sampler(ImageObject &image,
                    const vk::Filter min_filter,
                    const vk::Filter mag_filter,
                    const vk::SamplerMipmapMode mip_filter,
                    const vk::SamplerAddressMode address_mode_u,
                    const vk::SamplerAddressMode address_mode_v,
                    const vk::SamplerAddressMode address_mode_w);

void move_to_device(const BufferObject &source, ImageObject &dest,
                    const vk::Extent3D &extent);

void generate_mipmaps(ImageObject &image,
                      const vk::CommandBuffer &command_buffer);

void transition_layout(ImageObject &image,
                       const vk::ImageLayout old_layout,
                       const vk::ImageLayout new_layout,
                       const vk::CommandBuffer &command_buffer,
                       const uint32_t layer_count = 1u,
                       const uint32_t base_mip_level = 0u,
                       const uint32_t mip_levels = 1u);

void destroy_image(ImageObject &image);
void destroy_view(vk::ImageView &view);

} // namespace ImageTools
} // namespace vkl

#endif // VKLEARNIN_TOOLS_IMAGETOOLS_HPP