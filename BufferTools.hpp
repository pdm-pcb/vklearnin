#ifndef VKL_BUFFER_TOOLS_HPP
#define VKL_BUFFER_TOOLS_HPP

class Instance;

namespace BufferTools {

void create_buffer(::VkBuffer       &buffer,
                   const size_t      buffer_size,
                   const uint32_t    buffer_usage_flags,
                   ::VkDeviceMemory &memory,
                   const uint32_t    memory_flags,
                   const Instance   &instance);
} // namespace BufferTools

#endif // VKL_BUFFER_TOOLS_HPP