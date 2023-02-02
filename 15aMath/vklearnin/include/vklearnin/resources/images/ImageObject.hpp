#ifndef VKLEARNIN_RESOURCES_IMAGES_IMAGEOBJECT_HPP
#define VKLEARNIN_RESOURCES_IMAGES_IMAGEOBJECT_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct ImageObject final {
    vk::Image        handle { };
    vk::DeviceMemory memory { };
    vk::ImageView    view   { };
    vk::Format       format { vk::Format::eUndefined };
    vk::ImageLayout  layout { vk::ImageLayout::eUndefined };
    vk::Extent3D     extent { };
};

} // namespace vkl

#endif // VKLEARNIN_RESOURCES_IMAGES_IMAGEOBJECT_HPP