#ifndef VKLEARNIN_TOOLS_IMAGETOOLS_HPP
#define VKLEARNIN_TOOLS_IMAGETOOLS_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/shaders/ImageObject.hpp"

namespace vkl {

struct BufferObject;

namespace ImageTools {

ImageObject load_from_file(const char *filepath,
                           const bool flip_vertical = false);

ImageObject create_image(const vk::Extent3D &extent,
                         const vk::Format &color_format,
                         const vk::ImageAspectFlags &image_aspect,
                         const vk::ImageTiling &tiling,
                         const uint32_t mip_levels,
                         const vk::SampleCountFlagBits &sample_count,
                         const vk::ImageUsageFlags &usage,
                         const vk::MemoryPropertyFlags memory_properties);

void destroy_image(const ImageObject &image);

vk::ImageView create_view(const vk::Image &image,
                          const vk::Format &color_format,
                          const vk::ImageAspectFlags &image_aspect);

void destroy_view(const vk::ImageView &view);

vk::Sampler create_sampler(const vk::Filter min_filter,
                           const vk::Filter mag_filter,
                           const vk::SamplerMipmapMode mip_filter,
                           const vk::SamplerAddressMode address_mode_u,
                           const vk::SamplerAddressMode address_mode_v,
                           const vk::SamplerAddressMode address_mode_w);

void destroy_sampler(const vk::Sampler &sampler);

void transition_layout(ImageObject &image, const vk::ImageLayout new_layout);

void move_to_device(const BufferObject &source, ImageObject &dest,
                    const vk::Extent3D &extent);



} // namespace ImageTools
} // namespace vkl

#endif // VKLEARNIN_TOOLS_IMAGETOOLS_HPP