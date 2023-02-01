#ifndef VKLEARNIN_RESOURCES_IMAGES_IMAGETOOLS_HPP
#define VKLEARNIN_RESOURCES_IMAGES_IMAGETOOLS_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"
#include "vklearnin/resources/images/Texture2D.hpp"

namespace vkl::ImageTools {

void create_image(ImageObject &image,
                  const vk::ImageType type,
                  const vk::Extent3D extent,
                  const vk::SampleCountFlagBits samples,
                  const vk::ImageUsageFlags usage_flags,
                  const vk::MemoryPropertyFlags memory_properties);

void destroy_image(ImageObject &image);

void create_view(ImageObject &image,
                 const vk::ImageViewType view_type,
                 const vk::ImageAspectFlags &aspect_flags);

void destroy_view(ImageObject &image);

void* load_from_file(std::string_view filepath,
                     ImageObject      &image,
                     vk::Extent2D     &extent);

void free_file_data(void *data);

void host_to_device(ImageObject &dst,
                    const vk::Extent3D extent,
                    const void * const data);

vk::Sampler create_sampler(const ImageObject &image,
                           const vk::Filter min_filter,
                           const vk::Filter mag_filter,
                           const vk::SamplerAddressMode mode_u,
                           const vk::SamplerAddressMode mode_v);

void destroy_sampler(vk::Sampler &sampler);

} // namespace vkl::ImageTools

#endif // VKLEARNIN_RESOURCES_IMAGES_IMAGETOOLS_HPP