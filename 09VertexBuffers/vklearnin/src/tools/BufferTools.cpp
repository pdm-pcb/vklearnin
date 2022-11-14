#include "vklearnin/vklearnin.hpp"
#include "vklearnin/tools/BufferTools.hpp"

#include "vklearnin/rendering/PhysicalDevice.hpp"
#include "vklearnin/rendering/LogicalDevice.hpp"

namespace vkl {
namespace BufferTools {

uint32_t find_memory_type(const vk::MemoryPropertyFlags &flags,
                          const vk::MemoryRequirements &mem_reqs);

BufferObject create_buffer(size_t size_bytes,
                           const vk::BufferUsageFlags usage_flags,
                           const vk::SharingMode sharing_mode,
                           const vk::MemoryPropertyFlags memory_flags,
                           const vk::Device &logical_device)
{
    const auto &family_indices = PhysicalDevice::current().family_indices();
    vk::BufferCreateInfo buffer_info {
        .size                  = size_bytes,
        .usage                 = usage_flags,
        .sharingMode           = sharing_mode,
        .queueFamilyIndexCount =
            static_cast<uint32_t>(family_indices.size()),
        .pQueueFamilyIndices   = family_indices.data()
    };

    auto create_result = logical_device.createBuffer(buffer_info);
    if(create_result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to create {}-byte buffer", size_bytes);
        return { };
    }

    vk::Buffer buffer = create_result.value;
    
    vk::MemoryRequirements mem_reqs;
    logical_device.getBufferMemoryRequirements(buffer, &mem_reqs);
    uint32_t type_index = find_memory_type(memory_flags, mem_reqs);

    vk::MemoryAllocateInfo allocate_info {
        .allocationSize = mem_reqs.size,
        .memoryTypeIndex = type_index
    };

    auto alloc_result = logical_device.allocateMemory(allocate_info);

    if(alloc_result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to allocate {}-byte buffer", size_bytes);
    }
    
    CONSOLE_TRACE("Allocated {}-byte buffer", size_bytes);

    vk::DeviceMemory device_memory = alloc_result.value;

    auto bind_result =
        logical_device.bindBufferMemory(buffer, device_memory, 0u);

    if(bind_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to bind {}-byte buffer", size_bytes);
    }

    CONSOLE_TRACE(
        "Created buffer {}, device memory {}",
        reinterpret_cast<uint64_t>(VkBuffer(buffer)),
        reinterpret_cast<uint64_t>(VkDeviceMemory(device_memory))
    );

    return {
        .buffer = buffer,
        .memory = device_memory,
        .size   = size_bytes
    };
}

void destroy_buffer(BufferObject &buffer, const LogicalDevice &logical_device) {
    CONSOLE_TRACE(
        "Destroying buffer {}, memory {}",
        reinterpret_cast<uint64_t>(VkBuffer(buffer.buffer)),
        reinterpret_cast<uint64_t>(VkDeviceMemory(buffer.memory))
    );
    logical_device.native().destroy(buffer.buffer);
    logical_device.native().freeMemory(buffer.memory);
}

void fill_buffer(const void *data, const BufferObject &dest_buffer,
                 const LogicalDevice &logical_device)
{
    auto result = logical_device.native().mapMemory(
        dest_buffer.memory,
        0u,
        dest_buffer.size
    );

    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Could not map device memory {}",
            reinterpret_cast<uint64_t>(VkDeviceMemory(dest_buffer.memory))
        );
    }
    void *mapped_memory = result.value;
        memcpy(mapped_memory, data, dest_buffer.size);
    logical_device.native().unmapMemory(dest_buffer.memory);
}

uint32_t find_memory_type(const vk::MemoryPropertyFlags &flags,
                          const vk::MemoryRequirements &reqs)
{
    vk::PhysicalDeviceMemoryProperties mem_props;
    PhysicalDevice::current().native().getMemoryProperties(&mem_props);

    for(uint32_t type_index = 0;
        type_index < mem_props.memoryTypeCount;
        ++type_index)
    {
        if(reqs.memoryTypeBits & (1 << type_index)) {
            if(mem_props.memoryTypes[type_index].propertyFlags & flags) {
                return type_index;
            }
        }
    }

    return std::numeric_limits<uint32_t>::max();
}

} // namespace BufferTools
} // namespace vkl