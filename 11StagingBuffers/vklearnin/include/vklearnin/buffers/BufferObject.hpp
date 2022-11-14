#ifndef VKLEARNIN_MESH_BUFFEROBJECT_HPP
#define VKLEARNIN_MESH_BUFFEROBJECT_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct BufferObject {
    vk::Buffer       buffer;
    vk::DeviceMemory memory;
    size_t           size;
};

} // namespace vkl

#endif // VKLEARNIN_MESH_BUFFEROBJECT_HPP