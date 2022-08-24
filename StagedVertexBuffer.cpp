#include "common.hpp"
#include "StagedVertexBuffer.hpp"

#include "Instance.hpp"
#include "Vertex.hpp"

void StagedVertexBuffer::populate_buffer(const ::VkCommandPool &pool,
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

        VkBufferCopy copyRegion { };
        copyRegion.size = _buffer_size;
        ::vkCmdCopyBuffer(
            command_buffer,
            _staging_buffer,
            _vertex_buffer,
            1u,
            &copyRegion
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

    auto type_index = _mem_type_index(
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
StagedVertexBuffer::_mem_type_index(const uint32_t type_bits,
                                    const ::VkMemoryPropertyFlags flags)
{
    ::VkPhysicalDeviceMemoryProperties memory_props { };
    ::vkGetPhysicalDeviceMemoryProperties(
        _instance.physical_device(),
        &memory_props
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

void StagedVertexBuffer::_populate_staging_buffer() {
    ::vkMapMemory(
        _instance.logical_device(),
        _staging_memory,
        0u,
        _buffer_size,
        0u,
        &_staging_data
    );

    memcpy(_staging_data, _vertices.data(), _buffer_size);

    ::vkUnmapMemory(_instance.logical_device(), _staging_memory);
}

StagedVertexBuffer::StagedVertexBuffer(const std::vector<Vertex> &vertices,
                                       const Instance &instance) :
    _staging_buffer { nullptr },
    _staging_memory { nullptr },
    _staging_data   { nullptr },
    _vertex_buffer  { nullptr },
    _vertex_memory  { nullptr },
    _instance       { instance }
{
    _vertices.resize(vertices.size());
    _vertices = vertices;
    _buffer_size = sizeof(Vertex) * _vertices.size();

    _create_buffer(_staging_buffer, ::VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

    _allocate_memory(
        _staging_buffer,
        ::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        ::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        _staging_memory
    );
    
    _populate_staging_buffer();

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
}

StagedVertexBuffer::~StagedVertexBuffer() {
    ::vkDestroyBuffer(
        _instance.logical_device(),
        _staging_buffer,
        nullptr
    );

    ::vkFreeMemory(
        _instance.logical_device(),
        _staging_memory,
        nullptr
    );

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
}