#ifndef VKLEARNIN_RESOURCES_BUFFERS_VERTEXBUFFER_HPP
#define VKLEARNIN_RESOURCES_BUFFERS_VERTEXBUFFER_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/Vertex.hpp"

namespace vkl {

class VertexBuffer {
public:
    void create(const size_t size_bytes,
                const vk::BufferUsageFlags usage_flags,
                const vk::SharingMode sharing_mod,
                const vk::MemoryPropertyFlags memory_properties);
    void destroy();

    void populate_buffer(const std::vector<Vertex> &vertices);

    inline auto native() const { return _buffer; }

    VertexBuffer();
    ~VertexBuffer() = default;

    VertexBuffer(VertexBuffer &&) = delete;
    VertexBuffer(const VertexBuffer &) = delete;

    VertexBuffer & operator=(VertexBuffer &&) = delete;
    VertexBuffer & operator=(const VertexBuffer &) = delete;

private:
    size_t           _size;
    vk::Buffer       _buffer;
    vk::DeviceMemory _memory;

    void _allocate(const vk::MemoryPropertyFlags memory_properties);

    static uint32_t _find_memory_type(const vk::MemoryPropertyFlags flags,
                                      const vk::MemoryRequirements &reqs);
};

} // namespace vkl

#endif // VKLEARNIN_RESOURCES_BUFFERS_VERTEXBUFFER_HPP