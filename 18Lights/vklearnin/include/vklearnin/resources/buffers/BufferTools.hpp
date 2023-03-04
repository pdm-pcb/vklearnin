#ifndef VKLEARNIN_RESOURCES_BUFFERS_BUFFERTOOLS_HPP
#define VKLEARNIN_RESOURCES_BUFFERS_BUFFERTOOLS_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/buffers/BufferObject.hpp"

namespace vkl::BufferTools {

void create(BufferObject &buffer,
            const vk::BufferUsageFlags usage_flags,
            const vk::MemoryPropertyFlags memory_properties);

void destroy(BufferObject &buffer);

BufferObject stage_data(const size_t size, const void * const data);

void host_to_device(const BufferObject &dst, const void * const data);

void update_buffer(const BufferObject &buffer, const void * const data);

} // namespace vkl::BufferTools

#endif // VKLEARNIN_RESOURCES_BUFFERS_BUFFERTOOLS_HPP