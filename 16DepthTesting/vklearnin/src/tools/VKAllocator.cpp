#include "vklearnin/vklearnin.hpp"
#include "vklearnin/tools/VKAllocator.hpp"

#include "vklearnin/rendering/devices/PhysicalDevice.hpp"
#include "vklearnin/rendering/devices/LogicalDevice.hpp"

namespace vkl {

uint64_t VKAllocator::_page_size = 0u;
std::vector<VKAllocator::Pool> VKAllocator::_pools;
vk::PhysicalDeviceMemoryProperties VKAllocator::_memory_properties { };

// =============================================================================
VKAllocator::Alloc VKAllocator::allocate(
    const vk::Buffer &buffer,
    const vk::MemoryPropertyFlags memory_properties)
{
    vk::MemoryRequirements mem_reqs;
    LogicalDevice::native().getBufferMemoryRequirements(buffer, &mem_reqs);
    uint32_t type_index = _find_memory_type(memory_properties, mem_reqs);

    vk::MemoryAllocateInfo allocate_info {
        .allocationSize = mem_reqs.size,
        .memoryTypeIndex = type_index
    };

    auto alloc_result = LogicalDevice::native().allocateMemory(allocate_info);

    if(alloc_result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Failed to allocate buffer. {}",
            to_string(alloc_result.result)
        );
    }
    
    CONSOLE_TRACE("Allocated {} buffer", _size_string(mem_reqs.size));

    vk::DeviceMemory device_memory = alloc_result.value;

    auto bind_result =
        LogicalDevice::native().bindBufferMemory(buffer, device_memory, 0u);

    if(bind_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to bind buffer", to_string(bind_result));
    }

    CONSOLE_TRACE(
        "Created buffer {:#x}, and {:#x} device memory",
        reinterpret_cast<uint64_t>(VkBuffer(buffer)),
        reinterpret_cast<uint64_t>(VkDeviceMemory(device_memory))
    );

    _pools[type_index].active_used += 1u;
    _pools[type_index].total_used  += 1u;
    _pools[type_index].active_used_size += mem_reqs.size;
    _pools[type_index].total_used_size  += mem_reqs.size;

    CONSOLE_INFO(
        "{} / {} ({} in pool {})",
        _pools[type_index].active_used,
        _pools[type_index].total_used,
        _size_string(_pools[type_index].active_used_size),
        type_index
    );

    return {
        .memory = device_memory,
        .type   = type_index,
        .size   = mem_reqs.size,
        .offset = 0u,
        .block  = nullptr,
    };
}

// =============================================================================
VKAllocator::Alloc VKAllocator::allocate(
    const vk::Image &image,
    const vk::MemoryPropertyFlags memory_properties)
{    
    vk::MemoryRequirements mem_reqs;
    LogicalDevice::native().getImageMemoryRequirements(image, &mem_reqs);
    uint32_t type_index = _find_memory_type(memory_properties, mem_reqs);

    vk::MemoryAllocateInfo allocate_info {
        .allocationSize = mem_reqs.size,
        .memoryTypeIndex = type_index
    };

    auto alloc_result = LogicalDevice::native().allocateMemory(allocate_info);

    if(alloc_result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Failed to allocate image. {}",
            to_string(alloc_result.result)
        );
    }
    vk::DeviceMemory device_memory = alloc_result.value;

    auto bind_result =
        LogicalDevice::native().bindImageMemory(image, device_memory, 0u);

    if(bind_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to bind image. {}", to_string(bind_result));
    }

    CONSOLE_TRACE(
        "Created image {:#x}, and {:#x} device memory",
        reinterpret_cast<uint64_t>(VkImage(image)),
        reinterpret_cast<uint64_t>(VkDeviceMemory(device_memory))
    );

    _pools[type_index].active_used += 1u;
    _pools[type_index].total_used  += 1u;
    _pools[type_index].active_used_size += mem_reqs.size;
    _pools[type_index].total_used_size  += mem_reqs.size;

    CONSOLE_INFO(
        "{} / {} ({} in pool {})",
        _pools[type_index].active_used,
        _pools[type_index].total_used,
        _size_string(_pools[type_index].active_used_size),
        type_index
    );

    return {
        .memory = device_memory,
        .type   = type_index,
        .size   = mem_reqs.size,
        .offset = 0u,
        .block  = nullptr,
    };
}

// =============================================================================
void VKAllocator::free(const Alloc &allocation) {
    LogicalDevice::native().freeMemory(allocation.memory);

    _pools[allocation.type].active_used -= 1u;
    _pools[allocation.type].active_used_size -= allocation.size;

    CONSOLE_INFO(
        "{} / {} ({} bytes in pool {})",
        _pools[allocation.type].active_used,
        _pools[allocation.type].total_used,
        _size_string(_pools[allocation.type].active_used_size),
        allocation.type
    );
}

// =============================================================================
void VKAllocator::init() {
    PhysicalDevice::native().getMemoryProperties(&_memory_properties);
    CONSOLE_TRACE(
        "Found {} memory heaps; {} memory types",
        _memory_properties.memoryHeapCount,
        _memory_properties.memoryTypeCount
    );
    _pools.resize(_memory_properties.memoryTypeCount);

#ifdef VKL_DEBUG
    for(uint32_t heap = 0u; heap < _memory_properties.memoryHeapCount; ++heap) {
        const auto &mem_heap = _memory_properties.memoryHeaps[heap];
        _print_heap_flags(heap, mem_heap.flags);
    }
    for(uint32_t type = 0u; type < _memory_properties.memoryTypeCount; ++type) {
        const auto &mem_type = _memory_properties.memoryTypes[type];
        _print_memory_flags(type, mem_type.heapIndex, mem_type.propertyFlags);
    }
#endif // VKL_DEBUG

    const auto & device_props = PhysicalDevice::native().getProperties();
    _page_size = std::max(
        static_cast<uint64_t>(1024u),
        device_props.limits.bufferImageGranularity
    );

    CONSOLE_TRACE("VKAllocator page size: {}", _size_string(_page_size));
}

// =============================================================================
const std::string VKAllocator::_size_string(const uint64_t size) {
    if     (size < 1e3) { return fmt::format("{}b", size); }
    else if(size < 1e6) { return fmt::format("{:.1f}kb", size / 1.0e3f); }
    else if(size < 1e9) { return fmt::format("{:.1f}mb", size / 1.0e6f); }
    else                { return fmt::format("{:.1f}gb", size / 1.0e9f); }
}

// =============================================================================
void VKAllocator::_print_heap_flags(const uint32_t heap_index,
                                    const vk::MemoryHeapFlags &flags)
{
    std::stringstream flags_stream;
    flags_stream << "Heap " << heap_index << ": ";

    if(flags & vk::MemoryHeapFlagBits::eDeviceLocal) {
        flags_stream << "Device Local    ";
    }
    if(flags & vk::MemoryHeapFlagBits::eMultiInstance) {
        flags_stream << "Multi-instance  ";
    }

    if(static_cast<uint32_t>(flags) == 0u) {
        flags_stream << "Empty";
    }

    CONSOLE_TRACE("  {}", flags_stream.str());
}

// =============================================================================
void VKAllocator::_print_memory_flags(const uint32_t type_index,
                                      const uint32_t heap_index,
                                      const vk::MemoryPropertyFlags &flags)
{
    std::stringstream flags_stream;
    flags_stream << "Type " << type_index << " Heap " << heap_index << ": ";
    
    if(flags & vk::MemoryPropertyFlagBits::eHostCoherent) {
        flags_stream << "Host Coherent        ";
    }
    if(flags & vk::MemoryPropertyFlagBits::eHostVisible) {
        flags_stream << "Host Visible         ";
    }
    if(flags & vk::MemoryPropertyFlagBits::eHostCached) {
        flags_stream << "Host Cached          ";
    }
    if(flags & vk::MemoryPropertyFlagBits::eDeviceLocal) {
        flags_stream << "Device Local         ";
    }
    if(flags & vk::MemoryPropertyFlagBits::eLazilyAllocated) {
        flags_stream << "Lazily Allocated     ";
    }
    if(flags & vk::MemoryPropertyFlagBits::eProtected) {
        flags_stream << "Protected            ";
    }
    if(flags & vk::MemoryPropertyFlagBits::eDeviceUncachedAMD) {
        flags_stream << "Device Uncached AMD  ";
    }
    if(flags & vk::MemoryPropertyFlagBits::eDeviceCoherentAMD) {
        flags_stream << "Device Coherent AMD  ";
    }
    if(flags & vk::MemoryPropertyFlagBits::eRdmaCapableNV) {
        flags_stream << "RDMA Capable NV      ";
    }

    if(static_cast<uint32_t>(flags) == 0u) {
        flags_stream << "Empty";
    }

    CONSOLE_TRACE("  {}", flags_stream.str());
}

// =============================================================================
uint32_t VKAllocator::_find_memory_type(const vk::MemoryPropertyFlags &flags,
                                        const vk::MemoryRequirements &reqs)
{
    for(uint32_t type_index = 0;
        type_index < _memory_properties.memoryTypeCount;
        ++type_index)
    {
        if(reqs.memoryTypeBits & (1 << type_index)) {
            const auto &props = _memory_properties.memoryTypes[type_index];
            if(props.propertyFlags & flags) {
                return type_index;
            }
        }
    }

    return std::numeric_limits<uint32_t>::max();
}

} // namespace vkl