#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/VertexBuffer.hpp"

#include "vklearnin/system/devices/PhysicalDevice.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void VertexBuffer::create(const size_t size_bytes,
                          const vk::BufferUsageFlags usage_flags,
                          const vk::SharingMode sharing_mode,
                          const vk::MemoryPropertyFlags memory_properties)
{
    const vk::BufferCreateInfo buffer_info {
        .size        = size_bytes,
        .usage       = usage_flags,
        .sharingMode = sharing_mode,

        // Why should a generic block of memory care about device queue
        // families, you might ask? Provided this structure assures Vulkan we
        // do not intend to share this buffer between threads, the below are
        // simply ignored.
        .queueFamilyIndexCount = 0u,
        .pQueueFamilyIndices   = nullptr,
    };

    auto result = LogicalDevice::native().createBuffer(buffer_info);
    if(result.result != vk::Result::eSuccess || !result.value) {
        CONSOLE_CRITICAL(
            "Failed to create {}-byte vertex buffer: '{}'",
            size_bytes,
            to_string(result.result)
        );
        return;
    }

    CONSOLE_TRACE(
        "Created vertex buffer {:#x}",
        reinterpret_cast<uint64_t>(VkBuffer(result.value))
    );

    _buffer = result.value;
    _size = size_bytes;

    _allocate(memory_properties);
}

// =============================================================================
void VertexBuffer::destroy() {
    CONSOLE_TRACE(
        "\n\tDestroying vertex buffer {:#x}"
        "\n\tFreeing device memory {:#x}",
        reinterpret_cast<uint64_t>(VkBuffer(_buffer)),
        reinterpret_cast<uint64_t>(VkDeviceMemory(_memory))
    );

    LogicalDevice::native().destroyBuffer(_buffer);
    LogicalDevice::native().freeMemory(_memory);
}

// =============================================================================
void VertexBuffer::populate_buffer(const std::vector<Vertex> &vertices) {    
    // Asking the logical device to map a given memory handle provides us with
    // a destination for the memcpy() below
    auto result = LogicalDevice::native().mapMemory(_memory, 0u, _size);

    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Unable to map device memory {:#x}: '{}'",
            reinterpret_cast<uint64_t>(VkDeviceMemory(_memory)),
            to_string(result.result)
        );
        return;
    }

    memcpy(result.value, vertices.data(), _size);
    LogicalDevice::native().unmapMemory(_memory);
}

// =============================================================================
void VertexBuffer::_allocate(const vk::MemoryPropertyFlags memory_properties) {
    // The first order of business is to query the logical device about what
    // available memory matches properties we've specified thus far. A zero-
    // initialized vk::MemoryRequirements structure indicates that the
    // allocation we're after is the whole size of the buffer we've already
    // described with no offset.
    vk::MemoryRequirements mem_reqs { };
    LogicalDevice::native().getBufferMemoryRequirements(_buffer, &mem_reqs);

    // This function call will check the joint requirements of ourselves and
    // the logical device against the types of memory offered by the physical
    // device.
    auto type_index = _find_memory_type(memory_properties, mem_reqs);

    // Once a suitable memory type (and its index) is located, we're ready to
    // actually allocate the buffer.
    const vk::MemoryAllocateInfo alloc_info {
        .allocationSize = _size,
        .memoryTypeIndex = type_index,
    };

    auto alloc_result = LogicalDevice::native().allocateMemory(alloc_info);
    if(alloc_result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Failed to allocate {} bytes for vertex buffer {:#x}: '{}'",
            _size,
            reinterpret_cast<uint64_t>(VkBuffer(_buffer)),
            to_string(alloc_result.result)
        );
        return;
    }

    CONSOLE_TRACE(
        "\n\tAllocated {} bytes : {:#x}"
        "\n\tFor buffer {:#x}",
        _size,
        reinterpret_cast<uint64_t>(VkDeviceMemory(alloc_result.value)),
        reinterpret_cast<uint64_t>(VkBuffer(_buffer))
    );

    _memory = alloc_result.value;

    // Finally, 
    auto bind_result = LogicalDevice::native().bindBufferMemory(
        _buffer,
        _memory,
        0u
    );

    if(bind_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Binding attempt failed with '{}' for:"
            "\n\tVertex Buffer: {:#x}"
            "\n\tDevice Memory: {:#x}",
            to_string(bind_result),
            reinterpret_cast<uint64_t>(VkBuffer(_buffer)),
            reinterpret_cast<uint64_t>(VkDeviceMemory(_memory))
        );
    }
}

// =============================================================================
uint32_t VertexBuffer::_find_memory_type(const vk::MemoryPropertyFlags flags,
                                         const vk::MemoryRequirements &reqs)
{
    const auto &memory_properties = PhysicalDevice::memory_props();
    const auto type_count = memory_properties.memoryTypeCount;

    // This bit-rithmetic bears some explanation. We're checking two bit fields
    // against our requirements for the memory itself.
    for(uint32_t type_index = 0u; type_index < type_count; ++type_index) {

        // Each type index is actually a field in memoryTypeBits. If the index
        // we're currently on is enabled, that means we've found a matching
        // memory type.
        if((reqs.memoryTypeBits & (1u << type_index)) != 0u) {
            const auto &props = memory_properties.memoryTypes[type_index];

            // The second check is against the memory properties. This can be
            // any combination of local to the CPU, local to the GPU, visible
            // to the CPU or not, and more.
            if(props.propertyFlags & flags) {
                return type_index;
            }
        }
    }

    CONSOLE_CRITICAL("Could not find memory to match buffer requirements.");
    return std::numeric_limits<uint32_t>::max();
}

// =============================================================================
VertexBuffer::VertexBuffer() :
    _size   { 0 },
    _buffer { },
    _memory { }
{ }

} // namespace vkl