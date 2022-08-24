#include "common.hpp"
#include "VertexBuffer.hpp"

#include "Instance.hpp"

void VertexBuffer::_create_buffer() {
    VkBufferCreateInfo buffer_info { };
    buffer_info.sType = ::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size  = sizeof(Vertex) * _vertices.size();
    buffer_info.usage = ::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    buffer_info.sharingMode = ::VK_SHARING_MODE_EXCLUSIVE;

    auto result = ::vkCreateBuffer(
        _instance.logical_device(),
        &buffer_info,
        nullptr,
        &_buffer_handle
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_CRITICAL("Could not create vertex buffer.");
        return;
    }
}

void VertexBuffer::_allocate_memory() {
    ::VkMemoryRequirements memory_reqs { };
    ::vkGetBufferMemoryRequirements(
        _instance.logical_device(),
        _buffer_handle,
        &memory_reqs
    );

    uint32_t type_index = _mem_type_index(
        memory_reqs.memoryTypeBits,
        ::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        ::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    ::VkMemoryAllocateInfo alloc_info { };
    alloc_info.sType = ::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = memory_reqs.size;
    alloc_info.memoryTypeIndex = type_index;

    auto result = ::vkAllocateMemory(
        _instance.logical_device(),
        &alloc_info,
        nullptr,
        &_device_memory
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_ERROR("Unable to allocate device memory");
        return;
    }
}

uint32_t VertexBuffer::_mem_type_index(const uint32_t type_bits,
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

void VertexBuffer::_bind() const {
    ::vkBindBufferMemory(
        _instance.logical_device(),
        _buffer_handle,
        _device_memory,
        0u
    );
}

void VertexBuffer::_populate_buffer() {
    ::vkMapMemory(
        _instance.logical_device(),
        _device_memory,
        0u,
        _buffer_size,
        0u,
        &_buffer_data
    );

    memcpy(_buffer_data, _vertices.data(), _buffer_size);

    ::vkUnmapMemory(
        _instance.logical_device(),
        _device_memory
    );
}

VertexBuffer::VertexBuffer(const std::vector<Vertex> &vertices,
                           const Instance &instance) :
    _buffer_handle { nullptr  },
    _device_memory { nullptr  },
    _instance      { instance }
{
    _vertices.resize(vertices.size());
    _vertices = vertices;
    _buffer_size = sizeof(Vertex) * _vertices.size();

    _create_buffer();
    _allocate_memory();
    _bind();
    _populate_buffer();
}

VertexBuffer::~VertexBuffer() {
    ::vkDestroyBuffer(
        _instance.logical_device(),
        _buffer_handle,
        nullptr
    );

    ::vkFreeMemory(
        _instance.logical_device(),
        _device_memory,
        nullptr
    );
}