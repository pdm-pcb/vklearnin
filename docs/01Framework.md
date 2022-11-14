## Helpful Tools
We’ll be using one third party library in this section as well as two classes of our own making. The third party library is the excellent [`spdlog`](https://github.com/gabime/spdlog), which gives all we need from a logging utility, and more. The class we’ll implement is hardly more than a wrapper for `spdlog`, and it’ll be called `ConsoleLog`. The other class class we’ll be authoring is called `MemTracker`, which will help ensure that we haven’t leaked any memory during the lifetime of the program. They’re both optional, but I like to be reminded when I forget something as often as possible.

## Logging
`spdlog` can be downloaded and included in the header for `ConsoleLog`. If you'd like to use CMake's `FetchContent` to retrieve it, here's the target I'm using. The file occupies a new directory: `vklearnin/cmake/FetchDeps.cmake` .
```cmake
include(FetchContent)

message(NOTICE "Fetch spdlog")
FetchContent_Declare(
    spdlog 1.11.0
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG ad0e89cbfb4d0c1ce4d097e134eb7be67baebb36 # v1.11.0 | 2022-11-02
)
FetchContent_MakeAvailable(spdlog)
```

The above will be used by adding these lines to the top of `vklearnin/CMakeLists.txt`:
```cmake
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/")
include(FetchDeps)
```

The `ConsoleLog` declaration begins like this:
```cpp
#ifndef VKLEARNIN_CONSOLELOG_HPP
#define VKLEARNIN_CONSOLELOG_HPP

// This directive lets spdlog know we want to show every type of message, up to
// and including traces.
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h> // definitely want color, right?

#include <cassert>
```

Assertions are optional here, but I use them to make sure I can halt the program on a log entry of any given severity. The `ConsoleLog` class itself only has one proper function, `init()`.
```cpp
class ConsoleLog final {
public:
    static void init() {
        spdlog::set_level(spdlog::level::trace);
        // The format string requests color, time with milliseconds, thread ID,
        // and the name of the function in which the logging macro was expanded
        spdlog::set_pattern("%^[%T.%e][ %t ][%!()]: %v%$");
        SPDLOG_INFO("spdlog v{}.{}.{}", SPDLOG_VER_MAJOR,
                                        SPDLOG_VER_MINOR,
                                        SPDLOG_VER_PATCH);
    }

    ConsoleLog() = delete;
};
```

The actual "implementation" is just a bunch of macros that call `spdlog`'s existing macros.
```cpp
#define CONSOLE_TRACE(...)    SPDLOG_TRACE(__VA_ARGS__)
#define CONSOLE_INFO(...)     SPDLOG_INFO(__VA_ARGS__)
#define CONSOLE_WARN(...)     SPDLOG_WARN(__VA_ARGS__)
#define CONSOLE_ERROR(...)    SPDLOG_ERROR(__VA_ARGS__)
// I like to have anything I mark as critical immediately halt execution
#define CONSOLE_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__); assert(false)
```

## Catching Memory Leaks
The second tool to set up is `MemTracker`. With this code, we’re doing very coarse tracking every time memory is allocated and released. The goal is to ensure that an allocation of a given size has a matching number of both operations. All of this is permitted by simply wrapping the various permutations of `new` and `delete` in our own implementation and tacking the tracking onto the front of the operation.

Specifically, when an amount of memory is requested, the amount that’s allocated will be the bytes requested plus `sizeof(size_t)`. In the extra space up front, we store the size requested, and return a pointer to the remaining space, matching the user’s request.

When `delete` is called, we receive a pointer to the memory intended to be deleted. To recover the size (and keep our tallying up to date) we can imagine the memory location to be the second element in an array of `size_t`. This means an index of -1 will return the size in bytes originally requested. If a given call to `delete` results in zero total bytes allocated, then we must be at the end of the program _and_ all memory has been freed. Hooray!

The header file looks like this:
```cpp
#ifndef VKLEARNIN_MEMTRACKER_HPP
#define VKLEARNIN_MEMTRACKER_HPP

#include "vklearnin/pch.hpp"

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

#endif // VKLEARNIN_MEMTRACKER_HPP
```

And the implementation is as follows:
```cpp
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/MemTracker.hpp"

#ifdef VKL_DEBUG

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
    // imaginary array of size_t method is perfectly effective for my needs.
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
        printf(
            "\n~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ "
            "~ ~ ~ ~ ~ ~ ~ ~"
            "\nMission accomplished.\n"
            "~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ "
            "~ ~ ~ ~ ~ ~ ~\n\n"
        );
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

#endif // VKL_DEBUG
```

While this method is effective, it has some noteworthy drawbacks. For starters, you can’t know exactly which allocation of a given size wasn’t freed. I try to look for the celebration message at the end of any run so that I at least have a good guess as to what I most recently changed that’s leaked. More importantly, this method is useless if you’re using any third party software which will itself leak memory. This includes your GPU drivers, some of which leak quite badly on Linux, for example.

Even with those shortcomings, I still prefer to have this sort of validation in place for myself. There are as many variations on solving this problem as there are development toolchains, so employ your preferred method if this one doesn’t appeal.

One final note: the preprocessor directive `MEMLOG`. As you can see, it prints every time memory is allocated or released. This is an exceptionally noisy feature during initialization and shutdown, but where I find it handy is during ‘gameplay.’ Specifically, it becomes painfully obvious when you are allocating every frame, and you might be surprised what causes allocations unexpectedly.

### Testing the New Tools
The collection of headers in  `vklearnin/include/vklearnin/vklearnin.hpp` will grow with our new tools.
```cpp
// #define MEMLOG
#include "vklearnin/MemTracker.hpp"
#include "vklearnin/ConsoleLog.hpp"
#include "vklearnin/Application.hpp"
```

I've added a commented out `MEMLOG` so I remember where I intend to flip that switch when I want more verbosity.

Next, add a line `Application`'s constructor to initialize `ConsoleLog`:
```cpp
Application::Application() {
    ConsoleLog::init();
}
```

And that's it - compile and run. If you see something akin to the following as output, you're golden!

```bash
[11:27:56.299][ 16464 ][vkl::ConsoleLog::init()]: spdlog v1.11.0

~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
Mission accomplished.
~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
```

The first line confirms the version of `spdlog` while the second informs us that no memory leaks have taken place. Huzzah.