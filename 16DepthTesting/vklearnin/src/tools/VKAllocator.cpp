#include "vklearnin/vklearnin.hpp"
#include "vklearnin/tools/VKAllocator.hpp"

#include "vklearnin/rendering/devices/PhysicalDevice.hpp"
#include "vklearnin/rendering/devices/LogicalDevice.hpp"

namespace vkl {

uint32_t VKAllocator::_active_allocations = 0u;
uint32_t VKAllocator::_total_allocations  = 0u;

// =============================================================================
uint32_t find_memory_type(const vk::MemoryPropertyFlags &flags,
                          const vk::MemoryRequirements &reqs)
{
    vk::PhysicalDeviceMemoryProperties mem_props;
    PhysicalDevice::native().getMemoryProperties(&mem_props);

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

// =============================================================================
VKAllocator::Alloc VKAllocator::allocate(
    const vk::Buffer &buffer,
    const vk::MemoryPropertyFlags memory_properties)
{
    vk::MemoryRequirements mem_reqs;
    LogicalDevice::native().getBufferMemoryRequirements(buffer, &mem_reqs);
    uint32_t type_index = find_memory_type(memory_properties, mem_reqs);

    vk::MemoryAllocateInfo allocate_info {
        .allocationSize = mem_reqs.size,
        .memoryTypeIndex = type_index
    };

    auto alloc_result = LogicalDevice::native().allocateMemory(allocate_info);

    if(alloc_result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to allocate {}b buffer", mem_reqs.size);
    }
    
    if(mem_reqs.size < 1e3) {
        CONSOLE_TRACE("Allocated {}b buffer", mem_reqs.size);
    }
    else if(mem_reqs.size < 1e6) {
        CONSOLE_TRACE("Allocated {:.1f}kb buffer", mem_reqs.size / 1.0e3f);
    }
    else if(mem_reqs.size < 1e9) {
        CONSOLE_TRACE("Allocated {:.1f}mb buffer", mem_reqs.size / 1.0e6f);
    }
    else {
        CONSOLE_TRACE("Allocated {:.1f}gb buffer", mem_reqs.size / 1.0e9f);
    }

    vk::DeviceMemory device_memory = alloc_result.value;

    auto bind_result =
        LogicalDevice::native().bindBufferMemory(buffer, device_memory, 0u);

    if(bind_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to bind {}-byte buffer", mem_reqs.size);
    }

    CONSOLE_TRACE(
        "Created buffer {:#x}, device memory {:#x}",
        reinterpret_cast<uint64_t>(VkBuffer(buffer)),
        reinterpret_cast<uint64_t>(VkDeviceMemory(device_memory))
    );

    ++_active_allocations;
    ++_total_allocations;
    CONSOLE_INFO("{} / {}", _active_allocations, _total_allocations);

    return {
        .memory = device_memory,
        .id     = 0u,
        .size   = mem_reqs.size,
        .offset = 0,
    };
}

// =============================================================================
VKAllocator::Alloc VKAllocator::allocate(
    const vk::Image &image,
    const vk::MemoryPropertyFlags memory_properties)
{    
    vk::MemoryRequirements mem_reqs;
    LogicalDevice::native().getImageMemoryRequirements(image, &mem_reqs);
    uint32_t type_index = find_memory_type(memory_properties, mem_reqs);

    vk::MemoryAllocateInfo allocate_info {
        .allocationSize = mem_reqs.size,
        .memoryTypeIndex = type_index
    };

    auto alloc_result = LogicalDevice::native().allocateMemory(allocate_info);

    if(alloc_result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to allocate image");
    }
    vk::DeviceMemory device_memory = alloc_result.value;

    auto bind_result =
        LogicalDevice::native().bindImageMemory(image, device_memory, 0u);

    if(bind_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to bind image");
    }

    CONSOLE_TRACE(
        "Created image {:#x}, device memory {:#x}",
        reinterpret_cast<uint64_t>(VkImage(image)),
        reinterpret_cast<uint64_t>(VkDeviceMemory(device_memory))
    );

    ++_active_allocations;
    ++_total_allocations;
    CONSOLE_INFO("{} / {}", _active_allocations, _total_allocations);

    return {
        .memory = device_memory,
        .id     = 0u,
        .size   = mem_reqs.size,
        .offset = 0,
    };
}

// =============================================================================
void VKAllocator::free(const Alloc &allocation) {
    LogicalDevice::native().freeMemory(allocation.memory);

    --_active_allocations;
    CONSOLE_INFO("{} / {}", _active_allocations, _total_allocations);
}

} // namespace vkl