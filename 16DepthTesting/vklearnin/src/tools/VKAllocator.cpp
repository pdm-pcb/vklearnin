#include "vklearnin/vklearnin.hpp"
#include "vklearnin/tools/VKAllocator.hpp"

#include "vklearnin/rendering/devices/PhysicalDevice.hpp"
#include "vklearnin/rendering/devices/LogicalDevice.hpp"

namespace vkl {

const uint64_t VKAllocator::_block_size = 256u * 1024u * 1024u;
const uint8_t  VKAllocator::_max_blocks = 4u;
const uint64_t VKAllocator::_min_align  = 1024u;
std::vector<VKAllocator::Pool> VKAllocator::_pools;
vk::PhysicalDeviceMemoryProperties VKAllocator::_memory_properties { };

// =============================================================================
VKAllocator::Alloc VKAllocator::allocate(
    const vk::Buffer &buffer,
    const vk::MemoryPropertyFlags memory_properties)
{
    vk::MemoryRequirements mem_reqs { };
    LogicalDevice::native().getBufferMemoryRequirements(buffer, &mem_reqs);
    uint32_t type_index = _find_memory_type(memory_properties, mem_reqs);

    Alloc user_data {
        .size  = mem_reqs.size,
        .align = mem_reqs.alignment,
        .type  = type_index,
    };

    _find_free_block(user_data);

    auto &block = _pools[user_data.type].blocks[user_data.block_index];
    CONSOLE_TRACE(
        "Buffer {:#x}: {:#x} device memory, {} offset",
        reinterpret_cast<uint64_t>(VkBuffer(buffer)),
        reinterpret_cast<uint64_t>(VkDeviceMemory(block.memory)),
        user_data.offset
    );

    auto bind_result = LogicalDevice::native().bindBufferMemory(
        buffer,
        block.memory,
        user_data.offset
    );

    if(bind_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to bind buffer", to_string(bind_result));
    }

    return user_data;
}

// =============================================================================
VKAllocator::Alloc VKAllocator::allocate(
    const vk::Image &image,
    const vk::MemoryPropertyFlags memory_properties)
{    
    vk::MemoryRequirements mem_reqs { };
    LogicalDevice::native().getImageMemoryRequirements(image, &mem_reqs);
    uint32_t type_index = _find_memory_type(memory_properties, mem_reqs);

    Alloc user_data {
        .size  = mem_reqs.size,
        .align = mem_reqs.alignment,
        .type  = type_index,
    };

    _find_free_block(user_data);

    auto &block = _pools[user_data.type].blocks[user_data.block_index];
    CONSOLE_TRACE(
        "Image {:#x}: {:#x} device memory, {} offset",
        reinterpret_cast<uint64_t>(VkImage(image)),
        reinterpret_cast<uint64_t>(VkDeviceMemory(block.memory)),
        user_data.offset
    );

    auto bind_result = LogicalDevice::native().bindImageMemory(
        image,
        block.memory,
        user_data.offset
    );

    if(bind_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to bind image", to_string(bind_result));
    }

    return user_data;
}

// =============================================================================
void * VKAllocator::map_buffer(const Alloc &allocation) {
    auto &block = _pools[allocation.type].blocks[allocation.block_index];
    auto result = LogicalDevice::native().mapMemory(
        block.memory,
        allocation.offset,
        allocation.size
    );
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to map device memory");
    }

    return result.value;
}

// =============================================================================
void VKAllocator::unmap_buffer(const Alloc &allocation) {
    auto &block = _pools[allocation.type].blocks[allocation.block_index];
    LogicalDevice::native().unmapMemory(block.memory);
}

// =============================================================================
void VKAllocator::free(Alloc &allocation) {
    auto &blocks = _pools[allocation.type].blocks;
    auto &block = blocks[allocation.block_index];

    block.allocs.erase(allocation.alloc_iter);
    // Yeah... gotta do my own system-side allocator and then I'll come back
    // to this
    // block.free_size += allocation.size;
    // block.used_size -= allocation.size;
    
    allocation = Alloc { };
    _print_alloc_state();
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

    for(auto &pool : _pools) {
        pool.blocks.reserve(_max_blocks);
    }

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

    CONSOLE_TRACE("VKAllocator page size: {}", _size_string(_block_size));
    _print_alloc_state();
}

// =============================================================================
void VKAllocator::shutdown() {
    for(auto &pool : _pools) {
        for(auto &block : pool.blocks) {
            LogicalDevice::native().freeMemory(block.memory);
        }
    }
}

// =============================================================================
void VKAllocator::_find_free_block(Alloc &user_data) {
    if(user_data.align < _min_align) {
        user_data.align = _min_align;
    }

    auto size_reqd = ((user_data.size / user_data.align)) * user_data.align;
    if(size_reqd == 0u) {
        size_reqd = user_data.align;
    }

    CONSOLE_TRACE(
        "{:s} requested, {:s} alignment. {:s} required",
        _size_string(user_data.size),
        _size_string(user_data.align),
        _size_string(size_reqd)
    );

    auto &blocks = _pools[user_data.type].blocks;
    for(size_t block_index = 0; block_index < blocks.size(); ++block_index) {
        auto &block = blocks[block_index];
        if(block.free_size > size_reqd) {
            uint64_t aligned_offset =
                (block.used_size / user_data.align) * user_data.align;

            user_data.offset = aligned_offset;
            user_data.block_index = block_index;

            block.allocs.emplace_back(user_data);
            user_data.alloc_iter = std::prev(block.allocs.end());

            block.free_size -= size_reqd;
            block.used_size += size_reqd;

            break;
        }
    }

    if(user_data.block_index == std::numeric_limits<size_t>::max()) {
        assert(blocks.size() < _max_blocks);

        vk::MemoryAllocateInfo allocate_info {
            .allocationSize = _block_size,
            .memoryTypeIndex = user_data.type
        };
        
        auto result = LogicalDevice::native().allocateMemory(allocate_info);

        if(result.result != vk::Result::eSuccess) {
            CONSOLE_CRITICAL(
                "Failed to allocate new block for type {}. {}",
                user_data.type,
                to_string(result.result)
            );
        }
        
        CONSOLE_TRACE("Allocated new block for type {}.", user_data.type);

        user_data.offset = 0u;
        user_data.block_index = blocks.size();

        blocks.emplace_back(DeviceBlock {
            .memory = result.value,
            .free_size = _block_size,
            .used_size = 0u,
        });

        blocks.back().allocs.emplace_back(user_data);
        user_data.alloc_iter = blocks.back().allocs.begin();

        blocks.back().free_size -= size_reqd;
        blocks.back().used_size += size_reqd;
    }

    _print_alloc_state();
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

// =============================================================================
const std::string VKAllocator::_size_string(const uint64_t size) {
    static constexpr uint64_t kb = 1 << 10;
    static constexpr uint64_t mb = 1 << 20;
    static constexpr uint64_t gb = 1 << 30;

    static constexpr float   kbf = static_cast<float>(kb);
    static constexpr float   mbf = static_cast<float>(mb);
    static constexpr float   gbf = static_cast<float>(gb);

    if     (size < kb) { return fmt::format("{}b", size); }
    else if(size < mb) { return fmt::format("{:.1f}kb", size / kbf); }
    else if(size < gb) { return fmt::format("{:.2f}mb", size / mbf); }
    else               { return fmt::format("{:.3f}gb", size / gbf); }
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

void VKAllocator::_print_alloc_state() {
    // std::stringstream state_stream;

    // for(size_t pool_idx = 0; pool_idx < _pools.size(); ++pool_idx) {
    //     const auto &pool = _pools[pool_idx];
    //     state_stream << "\nPool " << pool_idx << ": " << pool.blocks.size()
    //                  << " blocks";

    //     for(size_t block_idx = 0; block_idx < pool.blocks.size(); ++block_idx) {
    //         const auto &block = pool.blocks[block_idx];

    //         state_stream << "\n  Block " << block_idx << ": "
    //                      << block.allocs.size() << " allocs. "
    //                      << _size_string(block.used_size) << " / "
    //                      << _size_string(block.free_size);

    //         for(const auto &alloc : block.allocs) {
    //             state_stream << "\n\tAllocation: " << _size_string(alloc.size)
    //                          << ",\tAlignment: "  << _size_string(alloc.align)
    //                          << ",\tOffset: " << _size_string(alloc.offset);
    //         }
    //     }
    // }

    // CONSOLE_INFO("{}", state_stream.str());
}

} // namespace vkl