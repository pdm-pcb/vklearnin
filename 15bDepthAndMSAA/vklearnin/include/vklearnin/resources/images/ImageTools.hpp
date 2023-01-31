#ifndef VKLEARNIN_RESOURCES_IMAGES_IMAGETOOLS_HPP
#define VKLEARNIN_RESOURCES_IMAGES_IMAGETOOLS_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"

namespace vkl::ImageTools {

void create_image(ImageObject &image, vk::ImageType type, vk::Extent3D extent,
                  vk::ImageUsageFlags usage_flags);
void destroy_image(ImageObject &image);

void create_view(ImageObject &image,
                 const vk::ImageViewType view_type,
                 const vk::ImageAspectFlags &aspect_flags);

void destroy_view(ImageObject &image);

} // namespace vkl::ImageTools

#endif // VKLEARNIN_RESOURCES_IMAGES_IMAGETOOLS_HPP