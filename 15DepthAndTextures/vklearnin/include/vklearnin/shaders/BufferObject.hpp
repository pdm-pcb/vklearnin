#ifndef VKLEARNIN_SHADERS_BUFFEROBJECT_HPP
#define VKLEARNIN_SHADERS_BUFFEROBJECT_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct BufferObject {
    vk::Buffer       buffer;
    vk::DeviceMemory memory;

    size_t size = 0u;
};

} // namespace vkl

#endif // VKLEARNIN_SHADERS_BUFFEROBJECT_HPP