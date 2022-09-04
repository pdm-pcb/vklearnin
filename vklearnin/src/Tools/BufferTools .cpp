#include "vklearnin/common.hpp"
#include "vklearnin/Tools/BufferTools.hpp"

#include "vklearnin/Instance.hpp"

namespace BufferTools {

// =============================================================================
void create_buffer(vk::Buffer &buffer, const size_t buffer_size,
                   const vk::BufferUsageFlags buffer_usage_flags,
                   ::VmaAllocation &memory, VmaMemoryUsage memory_usage,
                   uint32_t alloc_flags, const char *alloc_name)
{
    CONSOLE_INFO("");

    vk::BufferCreateInfo buffer_info {
        .size        = buffer_size,
        .usage       = buffer_usage_flags,
        .sharingMode = vk::SharingMode::eExclusive,
    };

    ::VmaAllocationCreateInfo vma_info {
        .flags = alloc_flags,
        .usage = memory_usage,
        .requiredFlags = 0u,
        .preferredFlags = 0u,
        .memoryTypeBits = 0u,
        .pool = nullptr,
        .pUserData = nullptr,
        .priority = 1.0f
    };

    ::vmaCreateBuffer(
        Allocator::allocator(),
        &static_cast<::VkBufferCreateInfo &>(buffer_info),
        &vma_info,
        &reinterpret_cast<::VkBuffer &>(buffer),
        &memory,
        nullptr
    );

    // CONSOLE_ERROR("Creating buffer {}", alloc_name);

    if(alloc_name != nullptr) {
        ::vmaSetAllocationName(
            Allocator::allocator(),
            memory,
            alloc_name
        );
    }

    // buffer = instance.logical_device().createBuffer(buffer_info);

    // allocate_memory(buffer, memory, memory_flags, instance);

    // ::vkBindBufferMemory(
    //     instance.logical_device(),
    //     buffer, memory,
    //     0u
    // );
}

void destroy_buffer(vk::Buffer &buffer, ::VmaAllocation &memory) {
    // ::VmaAllocationInfo info;
    // ::vmaGetAllocationInfo(Allocator::allocator(), memory, &info);
    // CONSOLE_ERROR("Destroying buffer {}", info.pName);

    ::vmaDestroyBuffer(Allocator::allocator(), buffer, memory);
}

/*
// =============================================================================
void allocate_memory(const vk::Buffer &buffer, vk::DeviceMemory &memory,
                     const vk::MemoryPropertyFlags type_flags,
                     const Instance &instance)
{
    CONSOLE_INFO("");

    auto memory_reqs =
        instance.logical_device().getBufferMemoryRequirements(buffer);

    auto type_index = find_memory_type(
        memory_reqs.memoryTypeBits,
        type_flags,
        instance
    );

    vk::MemoryAllocateInfo alloc_info { };
    alloc_info.allocationSize = memory_reqs.size;
    alloc_info.memoryTypeIndex = type_index;

    memory = instance.logical_device().allocateMemory(alloc_info);
}

// =============================================================================
uint32_t find_memory_type(const uint32_t type_bits,
                          const vk::MemoryPropertyFlags flags,
                          const Instance &instance)
{
    CONSOLE_INFO("");

    auto memory_props = instance.physical_device().getMemoryProperties();

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

    if(type_index > memory_props.memoryTypeCount) {
        CONSOLE_CRITICAL("Could not find matching memory type");
    }

    return type_index;
}
*/

} // namespace BufferTools
