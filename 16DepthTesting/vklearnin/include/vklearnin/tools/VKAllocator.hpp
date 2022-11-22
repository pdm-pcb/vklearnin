#ifndef VKLEARNIN_TOOLS_VKALLOCATOR_HPP
#define VKLEARNIN_TOOLS_VKALLOCATOR_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class VKAllocator final {
public:
    struct Block {
        uint64_t size   = 0u;
        bool above_free = false;
    };

    struct Alloc {
        vk::DeviceMemory memory { };
        uint32_t type   = 0u;
        uint64_t size   = 0u;
        uint64_t offset = 0u;

        Block *block = nullptr;
    };

    static Alloc allocate(const vk::Buffer &buffer,
                          const vk::MemoryPropertyFlags memory_properties);

    static Alloc allocate(const vk::Image &image,
                          const vk::MemoryPropertyFlags memory_properties);

    static void free(const Alloc &allocation);

    static void init();

    VKAllocator() = delete;
    ~VKAllocator() = delete;

    VKAllocator(VKAllocator &&other) = delete;
    VKAllocator(const VKAllocator &other) = delete;

    VKAllocator & operator=(VKAllocator &&other) = delete;
    VKAllocator & operator=(const VKAllocator &other) = delete;

private:
    struct Pool {
        std::list<Block> _free_blocks;
        std::list<Block> _used_blocks;

        uint64_t active_free      = 0u;
        uint64_t active_free_size = 0u;
        uint64_t active_used      = 0u;
        uint64_t active_used_size = 0u;

        uint64_t total_free       = 0u;
        uint64_t total_free_size  = 0u;
        uint64_t total_used       = 0u;
        uint64_t total_used_size  = 0u;
    };

    static uint64_t _page_size;
    static std::vector<Pool> _pools;

    static vk::PhysicalDeviceMemoryProperties _memory_properties;

    static const std::string _size_string(const uint64_t size);

    static void _print_heap_flags(const uint32_t heap_index,
                                  const vk::MemoryHeapFlags &flags);

    static void _print_memory_flags(const uint32_t type_index,
                                    const uint32_t heap_index,
                                    const vk::MemoryPropertyFlags &flags);

    static uint32_t _find_memory_type(const vk::MemoryPropertyFlags &flags,
                                      const vk::MemoryRequirements &reqs);
};

} // namespace vkl

#endif // VKLEARNIN_TOOLS_VKALLOCATOR_HPP