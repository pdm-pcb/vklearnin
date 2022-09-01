#ifndef VKLEARNIN_BUFFER_TOOLS_HPP
#define VKLEARNIN_BUFFER_TOOLS_HPP

#include <vulkan/vulkan.hpp>

#include <cstdint>

class Instance;

namespace BufferTools {

void create_buffer(vk::Buffer &buffer, const size_t buffer_size,
                   const vk::BufferUsageFlags buffer_usage_flags,
                   vk::DeviceMemory &memory,
                   const vk::MemoryPropertyFlags memory_flags,
                   const Instance &instance);

void allocate_memory(const vk::Buffer &buffer, vk::DeviceMemory &memory,
                     const vk::MemoryPropertyFlags type_flags,
                     const Instance &instance);

uint32_t find_memory_type(const uint32_t type_bits,
                          const vk::MemoryPropertyFlags flags,
                          const Instance &instance);
} // namespace BufferTools

#endif // VKLEARNIN_BUFFER_TOOLS_HPP