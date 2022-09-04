#ifndef VKLEARNIN_BUFFER_TOOLS_HPP
#define VKLEARNIN_BUFFER_TOOLS_HPP

#include "vklearnin/pch.hpp"
#include "vklearnin/Tools/Allocator.hpp"

namespace BufferTools {

void create_buffer(vk::Buffer &buffer, const size_t buffer_size,
                   const vk::BufferUsageFlags buffer_usage_flags,
                   VmaAllocation &memory, VmaMemoryUsage memory_usage,
                   uint32_t alloc_flags = 0u);

// void allocate_memory(const vk::Buffer &buffer, vk::DeviceMemory &memory,
//                      const vk::MemoryPropertyFlags type_flags,
//                      const Instance &instance);

// uint32_t find_memory_type(const uint32_t type_bits,
//                           const vk::MemoryPropertyFlags flags,
//                           const Instance &instance);
} // namespace BufferTools

#endif // VKLEARNIN_BUFFER_TOOLS_HPP