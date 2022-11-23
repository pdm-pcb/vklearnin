#ifndef VKLEARNIN_TOOLS_VKALLOCATOR_HPP
#define VKLEARNIN_TOOLS_VKALLOCATOR_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class VKAllocator final {
public:
    struct Alloc {
        uint64_t size   = 0u;
        uint64_t align  = 0u;
        uint64_t offset = 0u;
        uint32_t type   = 0u;
        
        size_t block_index = std::numeric_limits<size_t>::max();
        std::list<Alloc>::iterator alloc_iter;
    };

    static Alloc allocate(const vk::Buffer &buffer,
                          const vk::MemoryPropertyFlags memory_properties);

    static Alloc allocate(const vk::Image &image,
                          const vk::MemoryPropertyFlags memory_properties);

    static void * map_buffer(const Alloc &allocation);
    static void unmap_buffer(const Alloc &allocation);

    static void free(Alloc &allocation);

    static void init();
    static void shutdown();

    VKAllocator() = delete;
    ~VKAllocator() = delete;

    VKAllocator(VKAllocator &&other) = delete;
    VKAllocator(const VKAllocator &other) = delete;

    VKAllocator & operator=(VKAllocator &&other) = delete;
    VKAllocator & operator=(const VKAllocator &other) = delete;

private:
    struct DeviceBlock {
        vk::DeviceMemory memory { };
        
        uint64_t free_size = 0u;
        uint64_t used_size = 0u;

        std::list<Alloc> allocs;
    };

    struct Pool {
        std::vector<DeviceBlock> blocks;
    };

    static const uint64_t _block_size;
    static const uint8_t  _max_blocks;
    static std::vector<Pool> _pools;

    static vk::PhysicalDeviceMemoryProperties _memory_properties;

    static uint32_t _find_memory_type(const vk::MemoryPropertyFlags &flags,
                                      const vk::MemoryRequirements &reqs);

    static void _find_free_block(Alloc &user_data);

    static const std::string _size_string(const uint64_t size);

    static void _print_heap_flags(const uint32_t heap_index,
                                  const vk::MemoryHeapFlags &flags);

    static void _print_memory_flags(const uint32_t type_index,
                                    const uint32_t heap_index,
                                    const vk::MemoryPropertyFlags &flags);

    static void _print_alloc_state();
};

} // namespace vkl

#endif // VKLEARNIN_TOOLS_VKALLOCATOR_HPP