#ifndef VKLEARNIN_TOOLS_VKALLOCATOR_HPP
#define VKLEARNIN_TOOLS_VKALLOCATOR_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class VKAllocator final {
public:
    struct Alloc {
        vk::DeviceMemory memory { };
        uint32_t type = 0u;
        uint32_t id   = 0u;
        uint64_t size = 0;
        size_t offset = 0;
    };

    static Alloc allocate(const vk::Buffer &buffer,
                          const vk::MemoryPropertyFlags memory_properties);

    static Alloc allocate(const vk::Image &image,
                          const vk::MemoryPropertyFlags memory_properties);

    static void free(const Alloc &allocation);

    VKAllocator() = delete;
    ~VKAllocator() = delete;

    VKAllocator(VKAllocator &&other) = delete;
    VKAllocator(const VKAllocator &other) = delete;

    VKAllocator & operator=(VKAllocator &&other) = delete;
    VKAllocator & operator=(const VKAllocator &other) = delete;

private:
    static uint32_t _active_allocations;
    static uint32_t _total_allocations;
};

} // namespace vkl

#endif // VKLEARNIN_TOOLS_VKALLOCATOR_HPP