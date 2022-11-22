#ifndef VKLEARNIN_SHADERS_BUFFEROBJECT_HPP
#define VKLEARNIN_SHADERS_BUFFEROBJECT_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/tools/VKAllocator.hpp"

namespace vkl {

struct BufferObject {
    vk::Buffer buffer { };

    VKAllocator::Alloc allocation;
};

} // namespace vkl

#endif // VKLEARNIN_SHADERS_BUFFEROBJECT_HPP