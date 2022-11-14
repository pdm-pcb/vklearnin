#ifndef VKLEARNIN_TOOLS_IMAGETOOLS_HPP
#define VKLEARNIN_TOOLS_IMAGETOOLS_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {
namespace ImageTools {

void destroy_image(vk::Image &image);

vk::ImageView create_view(const vk::Image &image,
                          const vk::Format &color_format,
                          const vk::ImageAspectFlags &aspect_flags,
                          const vk::Device &device);

void destroy_view(vk::ImageView &view, const vk::Device &device);

} // namespace ImageTools
} // namespace vkl

#endif // VKLEARNIN_TOOLS_IMAGETOOLS_HPP