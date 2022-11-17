#ifndef VKLEARNIN_TOOLS_MEMTRACKER_HPP
#define VKLEARNIN_TOOLS_MEMTRACKER_HPP

#include "vklearnin/system/pch.hpp"

// Keeping tabs on news and deletes
struct MemTracker {
    static size_t total_bytes;
    static size_t alloc_count;
    static size_t free_count;
};

// Wrapping native memory management to facilitate the above tabs-keeping
#ifdef VKL_DEBUG
    void * operator new(size_t bytes);
    void * operator new[](size_t bytes);
    void * operator new(size_t size, const std::nothrow_t&) noexcept;

    void operator delete(void *memory)  noexcept;
    void operator delete[](void *memory) noexcept;
    void operator delete(void *memory, size_t)  noexcept;
    void operator delete[](void *memory, size_t) noexcept;
    void operator delete(void *memory, const std::nothrow_t&) noexcept;
#endif // VKL_DEBUG

#endif // VKLEARNIN_TOOLS_MEMTRACKER_HPP