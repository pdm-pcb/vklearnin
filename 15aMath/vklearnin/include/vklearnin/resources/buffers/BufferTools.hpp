#ifndef VKLEARNIN_RESOURCES_BUFFERS_BUFFERTOOLS_HPP
#define VKLEARNIN_RESOURCES_BUFFERS_BUFFERTOOLS_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct Buffer {
    size_t           size { 0 };
    vk::Buffer       handle { };
    vk::DeviceMemory memory { };
};

namespace BufferTools {

void create(const vk::BufferUsageFlags usage_flags,
            const vk::MemoryPropertyFlags memory_properties,
            Buffer &buffer);

void destroy(Buffer &buffer);

void host_to_device(const Buffer &dst_buffer, const void * const data);

} // namespace BufferTools

} // namespace vkl

#endif // VKLEARNIN_RESOURCES_BUFFERS_BUFFERTOOLS_HPP