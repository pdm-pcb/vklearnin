#ifndef VKLEARNIN_RESOURCES_IMAGES_IMAGEOBJECT_HPP
#define VKLEARNIN_RESOURCES_IMAGES_IMAGEOBJECT_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct ImageObject final {
    size_t               size         { 0 };
    vk::Extent3D         extent       { 0u, 0u, 0u };
    uint32_t             mip_levels   { 1u };
    uint32_t             array_layers { 1u };
    vk::Format           format       { vk::Format::eUndefined };
    vk::ImageLayout      layout       { vk::ImageLayout::eUndefined };
    vk::ImageAspectFlags aspect_flags { vk::ImageAspectFlagBits::eNone };
    vk::Image            handle       { };
    vk::DeviceMemory     memory       { };
    vk::ImageView        view         { };
    vk::Sampler          sampler      { };
};

} // namespace vkl

#endif // VKLEARNIN_RESOURCES_IMAGES_IMAGEOBJECT_HPP