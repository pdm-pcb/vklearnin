#ifndef VKLEARNIN_TOOLS_BUFFERTOOLS_HPP
#define VKLEARNIN_TOOLS_BUFFERTOOLS_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/shaders/BufferObject.hpp"

namespace vkl {

class LogicalDevice;

namespace BufferTools {

BufferObject create_buffer(size_t size_bytes,
                           const vk::BufferUsageFlags usage_flags,
                           const vk::SharingMode sharing_mode,
                           const vk::MemoryPropertyFlags memory_flags);

void destroy_buffer(BufferObject &buffer);

void fill_buffer(const void *data, const BufferObject &dest_buffer);

} // namespace BufferTools
} // namespace vkl

#endif // VKLEARNIN_TOOLS_BUFFERTOOLS_HPP