#ifndef VKL_STAGEDBUFFER_HPP
#define VKL_STAGEDBUFFER_HPP

#include <vulkan/vulkan.h>

#include <vector>
#include <cstdint>

class Instance;
class Vertex;

class StagedVertexBuffer final {
public:
    void populate_buffer(const ::VkCommandPool &pool, const ::VkQueue &queue);

    ::VkBuffer handle() const { return _vertex_buffer; }

    StagedVertexBuffer(const std::vector<Vertex> &vertices, const Instance &instance);
    ~StagedVertexBuffer();

    StagedVertexBuffer() = delete;

private:
    std::vector<Vertex> _vertices;
    size_t _buffer_size;

    ::VkBuffer       _staging_buffer;
    ::VkDeviceMemory _staging_memory;
    void            *_buffer_data;
    ::VkBuffer       _vertex_buffer;
    ::VkDeviceMemory _vertex_memory;

    const Instance  &_instance;

    void _create_buffer(::VkBuffer &handle, const uint32_t usage_flags);
    void _allocate_memory(const ::VkBuffer &handle, const uint32_t type_flags,
                          ::VkDeviceMemory &memory);
    uint32_t _mem_type_index(const uint32_t type_bits,
                             const ::VkMemoryPropertyFlags flags);
};

#endif // VKL_STAGEDBUFFER_HPP