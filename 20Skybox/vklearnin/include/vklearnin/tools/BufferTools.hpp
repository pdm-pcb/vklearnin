#ifndef VKLEARNIN_TOOLS_BUFFERTOOLS_HPP
#define VKLEARNIN_TOOLS_BUFFERTOOLS_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/shaders/BufferObject.hpp"

namespace vkl {

class LogicalDevice;

namespace BufferTools {

BufferObject create_buffer(const size_t size_bytes,
                           const vk::BufferUsageFlags usage_flags,
                           const vk::SharingMode sharing_mode,
                           const vk::MemoryPropertyFlags memory_properties,
                           std::string_view buffer_name);

vk::CommandBuffer begin_oneshot_cmd_buffer();
void end_oneshot_cmd_buffer(const vk::CommandBuffer &command_buffer);

BufferObject stage_data(const size_t size_bytes, const void *data);

void move_to_device(const void *data, const BufferObject &dest_buffer);

void destroy_buffer(BufferObject &buffer);


} // namespace BufferTools
} // namespace vkl

#endif // VKLEARNIN_TOOLS_BUFFERTOOLS_HPP