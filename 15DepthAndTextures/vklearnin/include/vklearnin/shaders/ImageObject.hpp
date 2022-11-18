#ifndef VKLEARNIN_SHADERS_IMAGEOBJECT_HPP
#define VKLEARNIN_SHADERS_IMAGEOBJECT_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct ImageObject {
    vk::Image        image;
    vk::ImageView    view;
    vk::DeviceMemory memory;
};

} // namespace vkl

#endif // VKLEARNIN_SHADERS_BUFFEROBJECT_HPP