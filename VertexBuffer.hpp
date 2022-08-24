#ifndef VKL_VERTEXBUFFER_HPP
#define VKL_VERTEXBUFFER_HPP

#include "Vertex.hpp"

#include <vulkan/vulkan.h>

#include <vector>
#include <cstdint>

class Instance;

class VertexBuffer final {
public:
    ::VkBuffer handle() const { return _buffer_handle; }

    VertexBuffer(const std::vector<Vertex> &vertices, const Instance &instance);
    ~VertexBuffer();

    VertexBuffer() = delete;

private:
    std::vector<Vertex> _vertices;

    ::VkBuffer       _buffer_handle;
    ::VkDeviceMemory _device_memory;
    void            *_buffer_data;

    const Instance  &_instance;

    void _create_buffer();
    void _populate_buffer();
    uint32_t _mem_type_index(const uint32_t type_bits,
                             const ::VkMemoryPropertyFlags flags);
};

#endif // VKL_VERTEXBUFFER_HPP