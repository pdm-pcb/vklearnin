#include "vklearnin/common.hpp"
#include "vklearnin/Tools/BufferTools.hpp"

#include "vklearnin/Instance.hpp"

namespace BufferTools {
void create_buffer(::VkBuffer       &buffer,
                   const size_t      buffer_size,
                   const uint32_t    buffer_usage_flags,
                   ::VkDeviceMemory &memory,
                   const uint32_t    memory_flags,
                   const Instance   &instance)
{
    CONSOLE_INFO("");

    ::VkBufferCreateInfo buffer_info { };
    buffer_info.sType       = ::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size        = buffer_size;
    buffer_info.usage       = buffer_usage_flags;
    buffer_info.sharingMode = ::VK_SHARING_MODE_EXCLUSIVE;

    auto result = ::vkCreateBuffer(
        instance.logical_device(),
        &buffer_info,
        nullptr,
        &buffer
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_ERROR("Failed to create buffer.");
    }

    allocate_memory(buffer, memory, memory_flags, instance);

    ::vkBindBufferMemory(
        instance.logical_device(),
        buffer, memory,
        0u
    );
}

void allocate_memory(const ::VkBuffer &buffer, ::VkDeviceMemory &memory,
                     const uint32_t type_flags, const Instance &instance) {
    CONSOLE_INFO("");

    ::VkMemoryRequirements memory_reqs { };
    ::vkGetBufferMemoryRequirements(
        instance.logical_device(),
        buffer,
        &memory_reqs
    );

    auto type_index = find_memory_type(memory_reqs.memoryTypeBits, type_flags,
                                       instance);

    ::VkMemoryAllocateInfo alloc_info { };
    alloc_info.sType = ::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = memory_reqs.size;
    alloc_info.memoryTypeIndex = type_index;

    auto result = ::vkAllocateMemory(
        instance.logical_device(),
        &alloc_info,
        nullptr,
        &memory
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_ERROR("Unable to allocate buffer memory");
        return;
    }
}

uint32_t find_memory_type(const uint32_t type_bits,
                          const ::VkMemoryPropertyFlags flags,
                          const Instance &instance)
{
    CONSOLE_INFO("");

    ::VkPhysicalDeviceMemoryProperties memory_props { };
    ::vkGetPhysicalDeviceMemoryProperties(
        instance.physical_device(),
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

    if(type_index > memory_props.memoryHeapCount) {
        CONSOLE_ERROR("Could not find matching memory type");
        return std::numeric_limits<uint32_t>::max();
    }

    return type_index;
}

} // namespace BufferTools
