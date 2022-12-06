#ifndef VKLEARNIN_SHADERS_IMAGEOBJECT_HPP
#define VKLEARNIN_SHADERS_IMAGEOBJECT_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/tools/VKAllocator.hpp"

namespace vkl {

struct ImageObject {
    vk::Image        image   { nullptr };
    vk::ImageView    view    { nullptr };
    vk::Format       format  { vk::Format::eUndefined };
    vk::ImageLayout  layout  { vk::ImageLayout::eUndefined };
    vk::Sampler      sampler { nullptr };

    uint32_t width    = 0u;
    uint32_t height   = 0u;
    uint32_t channels = 0u;
    size_t image_size = 0;
    size_t layer_size = 0; 

    uint32_t mip_levels = 1u;
    uint32_t array_layers = 1u;

    VKAllocator::BlockIter allocation;
};

} // namespace vkl

#endif // VKLEARNIN_SHADERS_BUFFEROBJECT_HPP