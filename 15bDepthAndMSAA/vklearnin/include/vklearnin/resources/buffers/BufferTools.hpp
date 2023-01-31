#ifndef VKLEARNIN_RESOURCES_BUFFERS_BUFFERTOOLS_HPP
#define VKLEARNIN_RESOURCES_BUFFERS_BUFFERTOOLS_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/buffers/BufferObject.hpp"

namespace vkl::BufferTools {

void create(const vk::BufferUsageFlags usage_flags,
            const vk::MemoryPropertyFlags memory_properties,
            BufferObject &buffer);

void destroy(BufferObject &buffer);

void host_to_device(const BufferObject &dst_buffer, const void * const data);

} // namespace vkl::BufferTools

#endif // VKLEARNIN_RESOURCES_BUFFERS_BUFFERTOOLS_HPP