#include "common.hpp"
#include "StagedVertexBuffer.hpp"

#include "Instance.hpp"
#include "Vertex.hpp"

void StagedVertexBuffer::populate_buffers(const ::VkCommandPool &pool,
                                          const ::VkQueue &queue)
{
    ::VkCommandBufferAllocateInfo alloc_info { };
    alloc_info.sType = ::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = ::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = pool;
    alloc_info.commandBufferCount = 1u;

    ::VkCommandBuffer command_buffer;
    auto result = ::vkAllocateCommandBuffers(
        _instance.logical_device(),
        &alloc_info,
        &command_buffer
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_CRITICAL("Failed to allocate vertex command buffer.");
        return;
    }

    ::VkCommandBufferBeginInfo buffer_info { };
    buffer_info.sType = ::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    buffer_info.flags = ::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    result = ::vkBeginCommandBuffer(command_buffer, &buffer_info);

    if(result != ::VK_SUCCESS) {
        CONSOLE_CRITICAL("Unable to begin vertex command buffer recording.");
        return;
    }

        ::VkBufferCopy vertex_region { };
        vertex_region.size = _vertex_buffer_size;
        ::vkCmdCopyBuffer(
            command_buffer,
            _staging_vertex_buffer,
            _vertex_buffer,
            1u,
            &vertex_region
        );

        ::VkBufferCopy index_region { };
        index_region.size = _index_buffer_size;
        ::vkCmdCopyBuffer(
            command_buffer,
            _staging_index_buffer,
            _index_buffer,
            1u,
            &index_region
        );

    result = ::vkEndCommandBuffer(command_buffer);
    if(result != ::VK_SUCCESS) {
        CONSOLE_CRITICAL("Failed to record to vertex command buffer.");
    }

    ::VkSubmitInfo submitInfo{};
    submitInfo.sType = ::VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1u;
    submitInfo.pCommandBuffers = &command_buffer;

    result = ::vkQueueSubmit(
        queue,
        1u,
        &submitInfo,
        nullptr
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_CRITICAL("Unable to submit vertex buffer command queue.");
    }

    ::vkQueueWaitIdle(queue);

    ::vkFreeCommandBuffers(
        _instance.logical_device(),
        pool,
        1u,
        &command_buffer
    );

    _destroy_staging_buffers();
}

void StagedVertexBuffer::_create_staging_buffers() {
    _create_buffer(_staging_vertex_buffer, ::VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    _allocate_memory(
        _staging_vertex_buffer,
        ::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        ::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        _staging_vertex_memory
    );

    _create_buffer(_staging_index_buffer, ::VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    _allocate_memory(
        _staging_index_buffer,
        ::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        ::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        _staging_index_memory
    );
}

void StagedVertexBuffer::_create_device_buffers() {
    _create_buffer(
        _vertex_buffer,
        ::VK_BUFFER_USAGE_TRANSFER_DST_BIT |
        ::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
    );

    _allocate_memory(
        _vertex_buffer,
        ::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        _vertex_memory
    );

    _create_buffer(
        _index_buffer,
        ::VK_BUFFER_USAGE_TRANSFER_DST_BIT |
        ::VK_BUFFER_USAGE_INDEX_BUFFER_BIT
    );

    _allocate_memory(
        _index_buffer,
        ::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        _index_memory
    );
}

void StagedVertexBuffer::_create_buffer(::VkBuffer &handle,
                                        const uint32_t usage_flags)
{
    VkBufferCreateInfo buffer_info { };
    buffer_info.sType = ::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size  = sizeof(Vertex) * _vertices.size();
    buffer_info.usage = usage_flags;
    buffer_info.sharingMode = ::VK_SHARING_MODE_EXCLUSIVE;

    auto result = ::vkCreateBuffer(
        _instance.logical_device(),
        &buffer_info,
        nullptr,
        &handle
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_ERROR("Failed to create buffer.");
    }
}

void StagedVertexBuffer::_allocate_memory(const ::VkBuffer &handle,
                                          const uint32_t type_flags,
                                          ::VkDeviceMemory &memory)
{
    ::VkMemoryRequirements memory_reqs { };
    ::vkGetBufferMemoryRequirements(
        _instance.logical_device(),
        handle,
        &memory_reqs
    );

    auto type_index = _find_memory_type(
        memory_reqs.memoryTypeBits,
        type_flags
    );

    ::VkMemoryAllocateInfo alloc_info { };
    alloc_info.sType = ::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = memory_reqs.size;
    alloc_info.memoryTypeIndex = type_index;

    auto result = ::vkAllocateMemory(
        _instance.logical_device(),
        &alloc_info,
        nullptr,
        &memory
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_ERROR("Unable to allocate buffer memory");
        return;
    }

    ::vkBindBufferMemory(
        _instance.logical_device(),
        handle,
        memory,
        0u
    );
}

uint32_t
StagedVertexBuffer::_find_memory_type(const uint32_t type_bits,
                                    const ::VkMemoryPropertyFlags flags)
{
    ::VkPhysicalDeviceMemoryProperties memory_props { };
    ::vkGetPhysicalDeviceMemoryProperties(
        _instance.physical_device(),
        &memory_props
    );

    CONSOLE_TRACE(
        "Found {} memory types and {} heaps on physical device.",
        memory_props.memoryTypeCount,
        memory_props.memoryHeapCount
    );

    uint32_t type_index = 0;
    while(type_index < memory_props.memoryTypeCount)
    {
        auto prop_flags = memory_props.memoryTypes[type_index].propertyFlags;
        if(type_bits & (1 << type_index) && (prop_flags & flags) == flags) {
            break;
        }
        ++type_index;
    }

    if(type_index == memory_props.memoryHeapCount) {
        CONSOLE_ERROR("Could not find matching memory type");
        return std::numeric_limits<uint32_t>::max();
    }

    return type_index;
}

void StagedVertexBuffer::_populate_staging_buffers() {
    ::vkMapMemory(
        _instance.logical_device(),
        _staging_vertex_memory,
        0u,
        _vertex_buffer_size,
        0u,
        &_staging_vertex_data
    );

    memcpy(_staging_vertex_data, _vertices.data(), _vertex_buffer_size);

    ::vkUnmapMemory(_instance.logical_device(), _staging_vertex_memory);

    ::vkMapMemory(
        _instance.logical_device(),
        _staging_index_memory,
        0u,
        _index_buffer_size,
        0u,
        &_staging_index_data
    );

    memcpy(_staging_index_data, _indices.data(), _index_buffer_size);

    ::vkUnmapMemory(_instance.logical_device(), _staging_index_memory);
}

void StagedVertexBuffer::_destroy_staging_buffers() {
    ::vkDestroyBuffer(
        _instance.logical_device(),
        _staging_vertex_buffer,
        nullptr
    );

    ::vkFreeMemory(
        _instance.logical_device(),
        _staging_vertex_memory,
        nullptr
    );

    ::vkDestroyBuffer(
        _instance.logical_device(),
        _staging_index_buffer,
        nullptr
    );

    ::vkFreeMemory(
        _instance.logical_device(),
        _staging_index_memory,
        nullptr
    );
}

StagedVertexBuffer::StagedVertexBuffer(const std::vector<Vertex> &vertices,
                                       const std::vector<Index> &indices,
                                       const Instance &instance) :
    _vertices           { vertices },
    _vertex_buffer_size { sizeof(Vertex) * _vertices.size() },
    _indices            { indices },
    _index_buffer_size  { sizeof(Index) * _indices.size() },
    _staging_vertex_buffer { nullptr },
    _staging_vertex_memory { nullptr },
    _staging_vertex_data   { nullptr },
    _vertex_buffer { nullptr },
    _vertex_memory { nullptr },
    _staging_index_buffer { nullptr },
    _staging_index_memory { nullptr },
    _staging_index_data   { nullptr },
    _index_buffer { nullptr  },
    _index_memory { nullptr  },
    _instance     { instance }
{
    _create_staging_buffers();
    _populate_staging_buffers();
    _create_device_buffers();
}

StagedVertexBuffer::~StagedVertexBuffer() {
    ::vkDestroyBuffer(
        _instance.logical_device(),
        _vertex_buffer,
        nullptr
    );

    ::vkFreeMemory(
        _instance.logical_device(),
        _vertex_memory,
        nullptr
    );

    ::vkDestroyBuffer(
        _instance.logical_device(),
        _index_buffer,
        nullptr
    );

    ::vkFreeMemory(
        _instance.logical_device(),
        _index_memory,
        nullptr
    );
}