#ifndef VKL_MEMTRACKER_HPP
#define VKL_MEMTRACKER_HPP

#include <cstddef>
#include <new>

struct MemTracker {
    static size_t total_bytes;
    static size_t alloc_count;
    static size_t free_count;
};

#ifdef DEBUG
    void * operator new(size_t bytes);
    void * operator new[](size_t bytes);
    // noexcept new
    void * operator new(size_t size, const std::nothrow_t& nothrow) noexcept;

    void operator delete(void *memory);
    void operator delete[](void *memory);
    void operator delete(void *memory, const std::nothrow_t& nothrow) noexcept;
#endif // DEBUG

#endif // VKL_MEMTRACKER_HPP