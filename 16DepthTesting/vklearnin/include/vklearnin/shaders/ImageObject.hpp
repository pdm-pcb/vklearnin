#ifndef VKLEARNIN_SHADERS_IMAGEOBJECT_HPP
#define VKLEARNIN_SHADERS_IMAGEOBJECT_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct ImageObject {
    vk::Image        image;
    vk::ImageView    view;
    vk::DeviceMemory memory;
    vk::Format       format;
    vk::ImageLayout  layout;
    vk::Sampler      sampler;

    uint32_t width      = 0u;
    uint32_t height     = 0u;
    uint32_t channels   = 0u;
    size_t   size_bytes = 0;
};

} // namespace vkl

#endif // VKLEARNIN_SHADERS_BUFFEROBJECT_HPP