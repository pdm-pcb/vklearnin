#ifndef VKLEARNIN_RESOURCES_IMAGES_IMAGEOBJECT_HPP
#define VKLEARNIN_RESOURCES_IMAGES_IMAGEOBJECT_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct ImageObject final {
    size_t           size { 0 };
    vk::Image        handle { };
    vk::DeviceMemory memory { };
    vk::ImageView    view   { };
    vk::Format       format { vk::Format::eUndefined };
    vk::ImageLayout  layout { vk::ImageLayout::eUndefined };
};

} // namespace vkl

#endif // VKLEARNIN_RESOURCES_IMAGES_IMAGEOBJECT_HPP