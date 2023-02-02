#ifndef VKLEARNIN_RESOURCES_IMAGES_IMAGETOOLS_HPP
#define VKLEARNIN_RESOURCES_IMAGES_IMAGETOOLS_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"

namespace vkl::ImageTools {

void create_image(ImageObject &image,
                  const vk::ImageType type,
                  const vk::SampleCountFlagBits samples,
                  const vk::ImageUsageFlags usage_flags,
                  const vk::MemoryPropertyFlags memory_properties);

void destroy_image(ImageObject &image);

void create_view(ImageObject &image,
                 const vk::ImageViewType view_type,
                 const vk::ImageAspectFlags &aspect_flags);

void destroy_view(ImageObject &image);

} // namespace vkl::ImageTools

#endif // VKLEARNIN_RESOURCES_IMAGES_IMAGETOOLS_HPP