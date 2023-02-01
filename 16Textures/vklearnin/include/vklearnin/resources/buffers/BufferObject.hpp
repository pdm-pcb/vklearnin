#ifndef VKLEARNIN_RESOURCES_BUFFERS_BUFFEROBJECT_HPP
#define VKLEARNIN_RESOURCES_BUFFERS_BUFFEROBJECT_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct BufferObject final {
    size_t           size { 0 };
    vk::Buffer       handle { };
    vk::DeviceMemory memory { };
};

} // namespace vkl

#endif // VKLEARNIN_RESOURCES_BUFFERS_BUFFEROBJECT_HPP