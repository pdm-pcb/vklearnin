#ifndef VKL_STAGEDBUFFER_HPP
#define VKL_STAGEDBUFFER_HPP

#include <vulkan/vulkan.h>

#include <vector>
#include <cstdint>

class Instance;
class Vertex;

class StagedVertexBuffer final {
public:
    using Index = uint32_t;

    void populate_buffers(const ::VkCommandPool &pool, const ::VkQueue &queue);

    inline ::VkBuffer vertex_handle() const { return _vertex_buffer; }
    inline ::VkBuffer index_handle()  const { return _index_buffer;  }

    inline uint32_t index_count() const {
        return static_cast<uint32_t>(_indices.size());
    }

    inline ::VkIndexType index_type() const {
        return ::VK_INDEX_TYPE_UINT32;
    }

    StagedVertexBuffer(const std::vector<Vertex> &vertices,
                       const std::vector<Index> &indices,
                       const Instance &instance);
    ~StagedVertexBuffer();

    StagedVertexBuffer() = delete;

private:
    std::vector<Vertex> _vertices;
    size_t _vertex_buffer_size;
    std::vector<Index> _indices;
    size_t _index_buffer_size;

    ::VkBuffer       _staging_vertex_buffer;
    ::VkDeviceMemory _staging_vertex_memory;
    void            *_staging_vertex_data;
    ::VkBuffer       _vertex_buffer;
    ::VkDeviceMemory _vertex_memory;

    ::VkBuffer       _staging_index_buffer;
    ::VkDeviceMemory _staging_index_memory;
    void            *_staging_index_data;
    ::VkBuffer       _index_buffer;
    ::VkDeviceMemory _index_memory;

    const Instance  &_instance;

    void _create_staging_buffers();
    void _populate_staging_buffers();
    void _create_device_buffers();
    void _destroy_staging_buffers();

    void _create_buffer(::VkBuffer &handle, const uint32_t usage_flags);
    void _allocate_memory(const ::VkBuffer &handle, const uint32_t type_flags,
                          ::VkDeviceMemory &memory);
    uint32_t _find_memory_type(const uint32_t type_bits,
                               const ::VkMemoryPropertyFlags flags);
};

#endif // VKL_STAGEDBUFFER_HPP