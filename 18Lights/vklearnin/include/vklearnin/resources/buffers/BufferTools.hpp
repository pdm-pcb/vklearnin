#ifndef VKLEARNIN_RESOURCES_BUFFERS_BUFFERTOOLS_HPP
#define VKLEARNIN_RESOURCES_BUFFERS_BUFFERTOOLS_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/buffers/BufferObject.hpp"

namespace vkl::BufferTools {

void create(BufferObject &buffer,
            vk::BufferUsageFlags const usage_flags,
            vk::MemoryPropertyFlags const memory_properties);

void destroy(BufferObject &buffer);

BufferObject stage_data(size_t const size, void const * const data);

void host_to_device(BufferObject const &dst, void const * const data);

void update_buffer(BufferObject const &buffer, void const * const data,
                   size_t const size);

} // namespace vkl::BufferTools

#endif // VKLEARNIN_RESOURCES_BUFFERS_BUFFERTOOLS_HPP