#include "common.hpp"
#include "MemTracker.hpp"

#ifdef DEBUG

size_t MemTracker::total_bytes = 0;
size_t MemTracker::alloc_count = 0;
size_t MemTracker::free_count  = 0;

void * operator new(size_t bytes) {
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
        printf("Tried to delete nullptr!\n");
        return;
    }
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
        printf(
            "\n~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ "
            "~ ~ ~ ~ ~ ~ ~ ~"
            "\nMission accomplished.\n"
            "~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ "
            "~ ~ ~ ~ ~ ~ ~\n\n"
        );
    }
}

void operator delete[](void *memory) noexcept {
    if(memory == nullptr) {
        printf("Tried to delete[] nullptr!\n");
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
        printf(
            "\n~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ "
            "~ ~ ~ ~ ~ ~ ~ ~"
            "\nMission accomplished.\n"
            "~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ "
            "~ ~ ~ ~ ~ ~ ~\n\n"
        );
    }
}


void operator delete(void *memory, std::size_t bytes) noexcept {
    if(memory == nullptr) {
        printf("Tried to sized delete nullptr!\n");
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
        printf(
            "\n~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ "
            "~ ~ ~ ~ ~ ~ ~ ~"
            "\nMission accomplished.\n"
            "~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ "
            "~ ~ ~ ~ ~ ~ ~\n\n"
        );
    }
}

void operator delete[](void *memory, std::size_t bytes) noexcept {
    if(memory == nullptr) {
        printf("Tried to sized delete[] nullptr!\n");
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
        printf(
            "\n~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ "
            "~ ~ ~ ~ ~ ~ ~ ~"
            "\nMission accomplished.\n"
            "~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ "
            "~ ~ ~ ~ ~ ~ ~\n\n"
        );
    }
}


void operator delete(void *memory, const std::nothrow_t&) noexcept {
    if(memory == nullptr) {
        printf("Tried to delete noexcept nullptr!\n");
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
        printf(
            "\n~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ "
            "~ ~ ~ ~ ~ ~ ~ ~"
            "\nMission accomplished.\n"
            "~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ "
            "~ ~ ~ ~ ~ ~ ~\n\n"
        );
    }
}

#endif // DEBUG