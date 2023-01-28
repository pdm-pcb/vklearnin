#include "vklearnin/vklearnin.hpp"
#include "vklearnin/tools/MemTracker.hpp"

#ifdef VKL_DEBUG

size_t MemTracker::total_bytes = 0;
size_t MemTracker::alloc_count = 0;
size_t MemTracker::free_count  = 0;

const char *success_string =
    "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
    "                         Mission accomplished"
    "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

void * operator new(size_t bytes) {
    MemTracker::total_bytes += bytes;
#ifdef MEMLOG
    printf(
        "%zu bytes allocated; %zu total, %zu / %zu\n",
        bytes, MemTracker::total_bytes, ++MemTracker::alloc_count,
        MemTracker::free_count
    );
#endif

    // How this works is by allocating sizeof(size_t) extra bytes on top of
    // what was requested by the user, then storing the actual requested size
    // in that extra space before returning a pointer to the rest of the space
    void *new_alloc = malloc(bytes + sizeof(size_t));
    static_cast<size_t *>(new_alloc)[0] = bytes;
    return &(static_cast<size_t *>(new_alloc)[1]);
}

void * operator new[](size_t bytes) {
    MemTracker::total_bytes += bytes;
#ifdef MEMLOG
    printf(
        "%zu bytes []allocated; %zu total, %zu / %zu\n",
        bytes, MemTracker::total_bytes, ++MemTracker::alloc_count,
        MemTracker::free_count
    );
#endif
    void *new_alloc = malloc(bytes + sizeof(size_t));
    static_cast<size_t *>(new_alloc)[0] = bytes;
    return &(static_cast<size_t *>(new_alloc)[1]);
}

void * operator new(size_t bytes, const std::nothrow_t&) noexcept {
    MemTracker::total_bytes += bytes;
#ifdef MEMLOG
    printf(
        "%zu bytes allocated; %zu total, %zu / %zu\n",
        bytes, MemTracker::total_bytes, ++MemTracker::alloc_count,
        MemTracker::free_count
    );
#endif

    void *new_alloc = malloc(bytes + sizeof(size_t));
    static_cast<size_t *>(new_alloc)[0] = bytes;
    return &(static_cast<size_t *>(new_alloc)[1]);
}

void operator delete(void *memory) noexcept {
    if(memory == nullptr) {
        return;
    }

    // When the user calls delete, we retrieve the original size requested via
    // the tricky -1 index. I didn't come up with this, but I'm struggling to
    // find the StackOverflow post to give proper credit. Either way, the
    // imaginary-array-of-size_t approach is perfect for my needs.
    size_t bytes = static_cast<size_t *>(memory)[-1];
    MemTracker::total_bytes -= bytes;

#ifdef MEMLOG
    printf(
        "%zu bytes freed; %zu total, %zu / %zu\n",
        bytes, MemTracker::total_bytes, MemTracker::alloc_count,
        ++MemTracker::free_count
    );
#endif

    free(&(static_cast<size_t *>(memory)[-1]));
    if(MemTracker::total_bytes == 0) {
        printf("%s", success_string);
    }
}

void operator delete[](void *memory) noexcept {
    if(memory == nullptr) {
        return;
    }

    size_t bytes = static_cast<size_t *>(memory)[-1];
    MemTracker::total_bytes -= bytes;

#ifdef MEMLOG
    printf(
        "%zu bytes []freed; %zu total, %zu / %zu\n",
        bytes, MemTracker::total_bytes, MemTracker::alloc_count,
        ++MemTracker::free_count
    );
#endif
    free(&(static_cast<size_t *>(memory)[-1]));
    if(MemTracker::total_bytes == 0) {
        printf("%s", success_string);
    }
}


void operator delete(void *memory, size_t bytes) noexcept {
    if(memory == nullptr) {
        return;
    }

    size_t expected_bytes = static_cast<size_t *>(memory)[-1];
    assert(expected_bytes == bytes);
    MemTracker::total_bytes -= bytes;

#ifdef MEMLOG
    printf(
        "%zu bytes freed; %zu total, %zu / %zu\n",
        bytes, MemTracker::total_bytes, MemTracker::alloc_count,
        ++MemTracker::free_count
    );
#endif

    free(&(static_cast<size_t *>(memory)[-1]));
    if(MemTracker::total_bytes == 0) {
        printf("%s", success_string);
    }
}

void operator delete[](void *memory, size_t bytes) noexcept {
    if(memory == nullptr) {
        return;
    }

    size_t expected_bytes = static_cast<size_t *>(memory)[-1];
    assert(expected_bytes == bytes);
    MemTracker::total_bytes -= bytes;

#ifdef MEMLOG
    printf(
        "%zu bytes []freed; %zu total, %zu / %zu\n",
        bytes, MemTracker::total_bytes, MemTracker::alloc_count,
        ++MemTracker::free_count
    );
#endif
    free(&(static_cast<size_t *>(memory)[-1]));
    if(MemTracker::total_bytes == 0) {
        printf("%s", success_string);
    }
}


void operator delete(void *memory, const std::nothrow_t&) noexcept {
    if(memory == nullptr) {
        return;
    }

    size_t bytes = static_cast<size_t *>(memory)[-1];
    MemTracker::total_bytes -= bytes;

#ifdef MEMLOG
    printf(
        "%zu bytes []freed; %zu total, %zu / %zu\n",
        bytes, MemTracker::total_bytes, MemTracker::alloc_count,
        ++MemTracker::free_count
    );
#endif
    free(&(static_cast<size_t *>(memory)[-1]));
    if(MemTracker::total_bytes == 0) {
        printf("%s", success_string);
    }
}

#endif // VKL_DEBUG