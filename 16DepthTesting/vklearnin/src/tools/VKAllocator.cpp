#include "vklearnin/vklearnin.hpp"
#include "vklearnin/tools/VKAllocator.hpp"

#include "vklearnin/rendering/devices/PhysicalDevice.hpp"
#include "vklearnin/rendering/devices/LogicalDevice.hpp"

namespace vkl {

const uint64_t VKAllocator::_max_alloc_size = 256u * 1024u * 1024u;
const uint8_t  VKAllocator::_max_allocs = 4u;
std::vector<VKAllocator::DevicePool> VKAllocator::_pools;
vk::PhysicalDeviceMemoryProperties VKAllocator::_memory_properties { };

// =============================================================================
VKAllocator::BlockIter VKAllocator::allocate(
    const vk::Buffer &buffer,
    const vk::MemoryPropertyFlags memory_properties)
{
    vk::MemoryRequirements mem_reqs { };
    LogicalDevice::native().getBufferMemoryRequirements(buffer, &mem_reqs);
    uint32_t type_index = _find_memory_type(memory_properties, mem_reqs);

    auto block_iter = _find_free_block(mem_reqs, type_index);

    CONSOLE_INFO("");
    _print_alloc_state();

    auto &alloc = _pools[type_index].allocs[block_iter->alloc_index];
    auto bind_result = LogicalDevice::native().bindBufferMemory(
        buffer,
        alloc.memory,
        block_iter->offset
    );

    if(bind_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to bind buffer", to_string(bind_result));
    }

    return block_iter;
}

// =============================================================================
VKAllocator::BlockIter VKAllocator::allocate(
    const vk::Image &image,
    const vk::MemoryPropertyFlags memory_properties)
{    
    vk::MemoryRequirements mem_reqs { };
    LogicalDevice::native().getImageMemoryRequirements(image, &mem_reqs);
    uint32_t type_index = _find_memory_type(memory_properties, mem_reqs);

    auto block_iter = _find_free_block(mem_reqs, type_index);

    CONSOLE_INFO("");
    _print_alloc_state();

    auto &alloc = _pools[type_index].allocs[block_iter->alloc_index];
    auto bind_result = LogicalDevice::native().bindImageMemory(
        image,
        alloc.memory,
        block_iter->offset
    );

    if(bind_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to bind image", to_string(bind_result));
    }

    return block_iter;
}

// =============================================================================
void * VKAllocator::map_buffer(const BlockIter &block_iter) {
    auto &block = _pools[block_iter->type_index].allocs[block_iter->alloc_index];
    auto result = LogicalDevice::native().mapMemory(
        block.memory,
        block_iter->offset,
        block_iter->size
    );
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to map device memory");
    }

    return result.value;
}

// =============================================================================
void VKAllocator::unmap_buffer(const BlockIter &block_iter) {
    auto &block = _pools[block_iter->type_index].allocs[block_iter->alloc_index];
    LogicalDevice::native().unmapMemory(block.memory);
}

// =============================================================================
void VKAllocator::free(BlockIter &block_iter) {
    auto &pool = _pools[block_iter->type_index];
    auto &alloc = pool.allocs[block_iter->alloc_index];
    auto &blocks = alloc.blocks;

    auto block_size = block_iter->size;

    CONSOLE_TRACE(
        "// free {} from pool {}, alloc {}",
        _size_string(block_size),
        block_iter->type_index,
        block_iter->alloc_index
    );

    BlockIter next_block = std::next(block_iter, 1);
    if(block_iter == blocks.begin() && next_block != blocks.end()) {
        CONSOLE_TRACE("// iter is begin and not end");
        if(!next_block->free) {
            CONSOLE_TRACE("// Nothing to do here but mark this block as free");
            block_iter->free = true;
        }
        else {
            CONSOLE_TRACE("// Absorb the block below");
            next_block->size   += block_iter->size;
            next_block->offset -= block_iter->size;

            blocks.erase(block_iter);
        }
    }
    else if(block_iter != blocks.begin() && next_block == blocks.end()) {
        CONSOLE_TRACE("// iter is not begin and is end");
        BlockIter prev_block = std::prev(block_iter, 1);
        if(!prev_block->free) {
            CONSOLE_TRACE("// Nothing to do here but mark this block as free");
            block_iter->free = true;
        }
        else {
            CONSOLE_TRACE("// Absorb the block above");
            prev_block->size += block_iter->size;

            blocks.erase(block_iter);
        }
    }
    else if(block_iter == blocks.begin() && next_block == blocks.end()) {
        CONSOLE_TRACE("// iter is both begin and end");
        CONSOLE_TRACE("// Nothing to do here but mark this block as free");
        block_iter->free = true;
    }
    else {
        CONSOLE_TRACE("// iter is neither begin and nor end");
        BlockIter prev_block = std::prev(block_iter, 1);
        if(prev_block->free && !next_block->free) {
            CONSOLE_TRACE("// Absorb the block above");
            prev_block->size += block_iter->size;

            blocks.erase(block_iter);
        }
        else if(!prev_block->free && next_block->free) {
            CONSOLE_TRACE("// Absorb the block below");
            next_block->size   += block_iter->size;
            next_block->offset -= block_iter->size;

            blocks.erase(block_iter);
        }
        else if(prev_block->free && next_block->free) {
            CONSOLE_TRACE("// Absorb both blocks. Wow!");
            prev_block->size += block_iter->size;
            prev_block->size += next_block->size;

            blocks.erase(next_block);
            blocks.erase(block_iter);
        }
        else {
            CONSOLE_TRACE("// Nothing to do here but mark this block as free");
            block_iter->free = true;
        }
    }

    alloc.used -= block_size;
    alloc.free += block_size;

    CONSOLE_INFO("");
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
        pool.allocs.reserve(_max_allocs);
    }

    CONSOLE_TRACE("VKAllocator alloc size: {}", _size_string(_max_alloc_size));

#ifdef VKL_DEBUG
    for(uint32_t heap = 0u; heap < _memory_properties.memoryHeapCount; ++heap) {
        const auto &mem_heap = _memory_properties.memoryHeaps[heap];
        _print_heap_flags(heap, mem_heap.flags);
    }
    for(uint32_t type = 0u; type < _memory_properties.memoryTypeCount; ++type) {
        const auto &mem_type = _memory_properties.memoryTypes[type];
        _print_memory_flags(type, mem_type.heapIndex, mem_type.propertyFlags);
    }

    CONSOLE_INFO("");
    _print_alloc_state();
#endif // VKL_DEBUG
}

// =============================================================================
void VKAllocator::shutdown() {
    for(auto &pool : _pools) {
        for(auto &dev_alloc : pool.allocs) {
            LogicalDevice::native().freeMemory(dev_alloc.memory);
        }
    }
}

// =============================================================================
VKAllocator::BlockIter VKAllocator::_find_free_block(
    const vk::MemoryRequirements &mem_reqs,
    const uint32_t type_index)
{
    auto aligned_size =
        ((mem_reqs.size / mem_reqs.alignment) + 1) * mem_reqs.alignment;

    CONSOLE_TRACE(
        "{:s} requested, {:s} alignment. {:s} required",
        _size_string(mem_reqs.size),
        _size_string(mem_reqs.alignment),
        _size_string(aligned_size)
    );

    assert(mem_reqs.size <= aligned_size);

    Block new_block {
        .type_index = type_index,
        .size = mem_reqs.size,
        .free = false
    };

    auto &allocs = _pools[type_index].allocs;
    for(size_t alloc_index = 0; alloc_index < allocs.size(); ++alloc_index)
    {
        auto &alloc = allocs[alloc_index];
        if(alloc.free >= aligned_size)
        {
            for(auto block = alloc.blocks.begin();
                block != alloc.blocks.end();
                std::advance(block, 1))
            {
                if(block->free && block->size >= aligned_size) {
                    uint64_t aligned_offset =
                        ((block->offset / mem_reqs.alignment) + 1) *
                        mem_reqs.alignment;

                    new_block.alloc_index = alloc_index;
                    new_block.offset = aligned_offset;

                    auto new_iter = alloc.blocks.emplace(block, new_block);

                    block->offset += aligned_size;
                    block->size   -= aligned_size;

                    if(block->size == 0u) {
                        alloc.blocks.erase(block);
                    }

                    alloc.used += aligned_size;
                    alloc.free -= aligned_size;

                    return new_iter;
                }
            }
        }
    }

    if(new_block.alloc_index == std::numeric_limits<size_t>::max()) {
        assert(allocs.size() < _max_allocs);

        vk::MemoryAllocateInfo allocate_info {
            .allocationSize = _max_alloc_size,
            .memoryTypeIndex = type_index
        };
        
        auto result = LogicalDevice::native().allocateMemory(allocate_info);

        if(result.result != vk::Result::eSuccess) {
            CONSOLE_CRITICAL(
                "Failed to allocate new device memory for type {}. {}",
                type_index,
                to_string(result.result)
            );
        }
        
        CONSOLE_TRACE("Allocated new device memory for type {}.", type_index);

        new_block.offset = 0u;
        new_block.alloc_index = allocs.size();

        allocs.emplace_back(DeviceAllocation {
            .memory = result.value,
            .free = _max_alloc_size,
            .used = 0u,
        });

        auto &alloc = allocs.back();

        Block fresh_block {
            .type_index  = type_index,
            .alloc_index = allocs.size() - 1,
            .size        = _max_alloc_size,
            .offset      = 0u,
            .free        = true,
        };

        alloc.blocks.emplace_front(fresh_block);
        auto current_block = alloc.blocks.begin();

        auto new_iter = alloc.blocks.emplace(current_block, new_block);

        current_block->offset += aligned_size;
        current_block->size -= aligned_size;

        if(current_block->size == 0u) {
            alloc.blocks.erase(current_block);
        }

        alloc.used += aligned_size;
        alloc.free -= aligned_size;

        return new_iter;
    }
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
    std::stringstream state_stream;

    for(size_t pool_idx = 0; pool_idx < _pools.size(); ++pool_idx)
    {
        const auto &pool = _pools[pool_idx];
        state_stream << std::format("\n  Pool {}", pool_idx);

        for(size_t alloc_idx = 0; alloc_idx < pool.allocs.size(); ++alloc_idx)
        {
            const auto &alloc = pool.allocs[alloc_idx];
            state_stream << std::format(
                "\n    Alloc {}: {} / {}; {} blocks",
                alloc_idx,
                _size_string(alloc.used),
                _size_string(alloc.free),
                alloc.blocks.size()
            );
            
            for(const auto &block : alloc.blocks)
            {
                state_stream << std::format(
                    "\n      offset {}\tsize {}\tfree {}",
                    _size_string(block.offset),
                    _size_string(block.size),
                    block.free
                );
            }

            state_stream << "\n";
        }
    }

    CONSOLE_INFO("{}", state_stream.str());
}

// =============================================================================
const std::string VKAllocator::_size_string(const uint64_t size) {
    static constexpr uint64_t kb = 1 << 10;
    static constexpr uint64_t mb = 1 << 20;
    static constexpr uint64_t gb = 1 << 30;

    static constexpr float   kbf = static_cast<float>(kb);
    static constexpr float   mbf = static_cast<float>(mb);
    static constexpr float   gbf = static_cast<float>(gb);

    if     (size < kb) { return fmt::format("{} b", size); }
    else if(size < mb) { return fmt::format("{:.1f}kb", size / kbf); }
    else if(size < gb) { return fmt::format("{:.2f}mb", size / mbf); }
    else               { return fmt::format("{:.3f}gb", size / gbf); }
}

} // namespace vkl