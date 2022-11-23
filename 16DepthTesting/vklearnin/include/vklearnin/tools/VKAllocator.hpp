#ifndef VKLEARNIN_TOOLS_VKALLOCATOR_HPP
#define VKLEARNIN_TOOLS_VKALLOCATOR_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class VKAllocator final {
public:
    struct Block {
        size_t type_index = std::numeric_limits<size_t>::max();
        size_t alloc_index = std::numeric_limits<size_t>::max();

        uint64_t size      = 0u;
        uint64_t offset    = 0u;
        
        bool free = true;
    };

    struct DeviceAllocation {
        vk::DeviceMemory memory { };
        
        uint64_t free = 0u;
        uint64_t used = 0u;

        std::list<Block> blocks;
    };

    struct DevicePool {
        std::vector<DeviceAllocation> allocs;
    };

    using BlockIter = std::list<Block>::iterator;

    static BlockIter allocate(const vk::Buffer &buffer,
                             const vk::MemoryPropertyFlags memory_properties);

    static BlockIter allocate(const vk::Image &image,
                             const vk::MemoryPropertyFlags memory_properties);

    static void * map_buffer(const BlockIter &block_iter);
    static void unmap_buffer(const BlockIter &block_iter);

    static void free(BlockIter &block_iter);

    static void init();
    static void shutdown();

    VKAllocator() = delete;
    ~VKAllocator() = delete;

    VKAllocator(VKAllocator &&other) = delete;
    VKAllocator(const VKAllocator &other) = delete;

    VKAllocator & operator=(VKAllocator &&other) = delete;
    VKAllocator & operator=(const VKAllocator &other) = delete;

private:
    static const uint64_t _max_alloc_size;
    static const uint8_t  _max_allocs;
    static std::vector<DevicePool> _pools;

    static vk::PhysicalDeviceMemoryProperties _memory_properties;

    static uint32_t _find_memory_type(const vk::MemoryPropertyFlags &flags,
                                      const vk::MemoryRequirements &reqs);

    static BlockIter _find_free_block(const vk::MemoryRequirements &mem_reqs,
                                      const uint32_t type_index);

    static void _print_heap_flags(const uint32_t heap_index,
                                  const vk::MemoryHeapFlags &flags);

    static void _print_memory_flags(const uint32_t type_index,
                                    const uint32_t heap_index,
                                    const vk::MemoryPropertyFlags &flags);

    static void _print_alloc_state();

    static const std::string _size_string(const uint64_t size);
};

} // namespace vkl

#endif // VKLEARNIN_TOOLS_VKALLOCATOR_HPP