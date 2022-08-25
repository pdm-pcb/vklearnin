#ifndef VKLEARNIN_MEMTRACKER_HPP
#define VKLEARNIN_MEMTRACKER_HPP

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
    void * operator new(size_t size, const std::nothrow_t&) noexcept;

    void operator delete(void *memory)  noexcept;
    void operator delete[](void *memory) noexcept;
    void operator delete(void *memory, std::size_t)  noexcept;
    void operator delete[](void *memory, std::size_t) noexcept;
    void operator delete(void *memory, const std::nothrow_t&) noexcept;
#endif // DEBUG

#endif // VKLEARNIN_MEMTRACKER_HPP