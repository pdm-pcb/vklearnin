## Helpful Tools
We’ll be using one third party library in this section as well as two classes of our own making. The library in question is the excellent [`spdlog`](https://github.com/gabime/spdlog), which gives all we need from a logging utility, and more. The class we’ll implement is hardly more than a wrapper for `spdlog`, and it’ll be called `ConsoleLog`. The other class class we’ll be authoring is called `MemTracker`, which will help ensure that we haven’t leaked any memory during the lifetime of the program. They’re both optional, but I like to be reminded when I've forgotten something as often as possible.

One thing before starting - `vklearnin/system/pch.hpp` will need the following couple of lines.

```cpp
#ifndef VKLEARNIN_SYSTEM_PCH_HPP
#define VKLEARNIN_SYSTEM_PCH_HPP

#include <cstdio>
#include <cassert>

#include <new>

#endif // VKLEARNIN_SYSTEM_PCH_HPP
```

## Logging
`spdlog` can be downloaded and included in the header for `ConsoleLog`. If you'd like to use CMake's `FetchContent` to retrieve it, here's the target I'm using. The file occupies a new directory: `vklearnin/cmake/FetchDeps.cmake` .

```cmake
include(FetchContent)

message(NOTICE "Fetch spdlog")
FetchContent_Declare(
    spdlog 1.11.0 # 2022-11-02
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG ad0e89cbfb4d0c1ce4d097e134eb7be67baebb36
	FIND_PACKAGE_ARGS # prefer local installation when available
)
FetchContent_MakeAvailable(spdlog)
```

The `ConsoleLog` declaration goes like this:

```cpp
#ifndef VKLEARNIN_TOOLS_CONSOLELOG_HPP
#define VKLEARNIN_TOOLS_CONSOLELOG_HPP

#include "vklearnin/system/pch.hpp"

// This directive lets spdlog know we want to show every type of message, up to
// and including traces.
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h> // definitely want color, right?

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

#define CONSOLE_TRACE(...)    SPDLOG_TRACE(__VA_ARGS__)
#define CONSOLE_INFO(...)     SPDLOG_INFO(__VA_ARGS__)
#define CONSOLE_WARN(...)     SPDLOG_WARN(__VA_ARGS__)
#define CONSOLE_ERROR(...)    SPDLOG_ERROR(__VA_ARGS__)
// I like to have anything I mark as critical immediately halt execution
#define CONSOLE_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__); assert(false)

} // namespace vkl
#endif // VKLEARNIN_TOOLS_CONSOLELOG_HPP
```

## Catching Memory Leaks
The second tool to set up is `MemTracker`. With this code, we’re doing very coarse tracking every time memory is allocated and released. The goal is to ensure that an allocation of a given size has a matching number of both operations. All of this is permitted by simply wrapping the various permutations of `new` and `delete` in our own implementation and tacking the tracking onto the front of the operation.

Specifically, when an amount of memory is requested, the amount that’s allocated will be the bytes requested plus `sizeof(size_t)`. In the extra space up front, we store the size requested, and return a pointer to the remaining space, matching the user’s request.

When `delete` is called, we receive a pointer to the memory intended to be deleted. To recover the size (and keep our tallying up to date) we can imagine the memory location to be the second element in an array of `size_t`. This means an index of -1 will return the size in bytes originally requested. If a given call to `delete` results in zero total bytes allocated, then we must be at the end of the program _and_ all memory has been freed. Hooray!

The header file looks like this:

```cpp
#ifndef VKLEARNIN_TOOLS_MEMTRACKER_HPP
#define VKLEARNIN_TOOLS_MEMTRACKER_HPP

// Wrapping native memory management to facilitate the above tabs-keeping
#ifdef VKL_DEBUG

#include "vklearnin/system/pch.hpp"

    // Keeping tabs on news and deletes
    struct MemTracker {
        static size_t total_bytes;
        static size_t alloc_count;
        static size_t free_count;
    };

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
```

And the implementation is as follows:

```cpp
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
#ifdef VKL_MEMLOG
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
#ifdef VKL_MEMLOG
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
#ifdef VKL_MEMLOG
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

#ifdef VKL_MEMLOG
    printf(
        "%zu bytes freed; %zu total, %zu / %zu\n",
        bytes, MemTracker::total_bytes, MemTracker::alloc_count,
        ++MemTracker::free_count
    );
#endif

    free(&(static_cast<size_t *>(memory)[-1]));
    if(MemTracker::total_bytes == 0) {
        printf(success_string);
    }
}

void operator delete[](void *memory) noexcept {
    if(memory == nullptr) {
        return;
    }

    size_t bytes = static_cast<size_t *>(memory)[-1];
    MemTracker::total_bytes -= bytes;

#ifdef VKL_MEMLOG
    printf(
        "%zu bytes []freed; %zu total, %zu / %zu\n",
        bytes, MemTracker::total_bytes, MemTracker::alloc_count,
        ++MemTracker::free_count
    );
#endif
    free(&(static_cast<size_t *>(memory)[-1]));
    if(MemTracker::total_bytes == 0) {
        printf(success_string);
    }
}


void operator delete(void *memory, size_t bytes) noexcept {
    if(memory == nullptr) {
        return;
    }

    size_t expected_bytes = static_cast<size_t *>(memory)[-1];
    assert(expected_bytes == bytes);
    MemTracker::total_bytes -= bytes;

#ifdef VKL_MEMLOG
    printf(
        "%zu bytes freed; %zu total, %zu / %zu\n",
        bytes, MemTracker::total_bytes, MemTracker::alloc_count,
        ++MemTracker::free_count
    );
#endif

    free(&(static_cast<size_t *>(memory)[-1]));
    if(MemTracker::total_bytes == 0) {
        printf(success_string);
    }
}

void operator delete[](void *memory, size_t bytes) noexcept {
    if(memory == nullptr) {
        return;
    }

    size_t expected_bytes = static_cast<size_t *>(memory)[-1];
    assert(expected_bytes == bytes);
    MemTracker::total_bytes -= bytes;

#ifdef VKL_MEMLOG
    printf(
        "%zu bytes []freed; %zu total, %zu / %zu\n",
        bytes, MemTracker::total_bytes, MemTracker::alloc_count,
        ++MemTracker::free_count
    );
#endif
    free(&(static_cast<size_t *>(memory)[-1]));
    if(MemTracker::total_bytes == 0) {
        printf(success_string);
    }
}


void operator delete(void *memory, const std::nothrow_t&) noexcept {
    if(memory == nullptr) {
        return;
    }

    size_t bytes = static_cast<size_t *>(memory)[-1];
    MemTracker::total_bytes -= bytes;

#ifdef VKL_MEMLOG
    printf(
        "%zu bytes []freed; %zu total, %zu / %zu\n",
        bytes, MemTracker::total_bytes, MemTracker::alloc_count,
        ++MemTracker::free_count
    );
#endif
    free(&(static_cast<size_t *>(memory)[-1]));
    if(MemTracker::total_bytes == 0) {
        printf(success_string);
    }
}

#endif // VKL_DEBUG
```

While this method is effective, it has some noteworthy drawbacks. For starters, you can’t know exactly which allocation of a given size wasn’t freed. I try to look for the celebration message at the end of any run so that I at least have a good guess as to what I most recently changed that’s leaked. More importantly, this method is useless if you’re using any third party software which will itself leak memory. This includes your GPU drivers, some of which leak quite badly on Linux, for example.

Even with those shortcomings, I still prefer to have this sort of validation in place for myself. There are as many variations on solving this problem as there are development toolchains, so employ your preferred method if this one doesn’t appeal.

One final note: the preprocessor directive `VKL_MEMLOG`. As you can see, it prints every time memory is allocated or released. This is an exceptionally noisy feature during initialization and shutdown, but where I find it handy is during ‘gameplay.’ Specifically, it becomes painfully obvious when you are allocating every frame, and you might be surprised what causes allocations unexpectedly.

## Testing, Round One
Let's define `Application` and the user-side `Demo` classes for which we've created files. `Application.hpp` will only povide a `run()` function for now, as well as some pure virtual functions.

```cpp
#ifndef VKLEARNIN_SYSTEM_APPLICATION_HPP
#define VKLEARNIN_SYSTEM_APPLICATION_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Application {
public:
    // Main loop
    void run();

    // To be overridden by user code
    virtual void init() = 0;
    virtual void shutdown() = 0;

    Application();
    virtual ~Application();

    Application(Application &&) = delete;
    Application(const Application &) = delete;

    Application & operator=(Application &&) = delete;
    Application & operator=(const Application &) = delete;
};

} // namespace vkl
#endif // VKLEARNIN_SYSTEM_APPLICATION_HPP
```

`Application.cpp` will be similarly sparse:

```cpp
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/Application.hpp"

namespace vkl {

// =============================================================================
void Application::run() {

}

// =============================================================================
Application::Application() {
    ConsoleLog::init();
}

Application::~Application() {

}

} // namespace vkl
```

Before jumping over to the user code, `vklearnin.hpp` needs some love.

```cpp
#ifndef VKLEARNIN_VKLEARNIN_HPP
#define VKLEARNIN_VKLEARNIN_HPP

#include "vklearnin/system/Application.hpp"

#define VKL_MEMLOG
#include "vklearnin/tools/MemTracker.hpp"
#include "vklearnin/tools/ConsoleLog.hpp"

#endif // VKLEARNIN_VKLEARNIN_HPP
```

With all of that laid down, it's time for the bare minimum `Demo` class. Starting with the declaration, as usual:

```cpp
#ifndef DEMO_HPP
#define DEMO_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/system/Application.hpp"

class Demo : public vkl::Application {
public:
    void init() override { }
    void shutdown() override { }

    Demo();
    ~Demo() = default;

    Demo(Demo &&) = delete;
    Demo(const Demo &) = delete;

    Demo & operator=(Demo &&) = delete;
    Demo & operator=(const Demo &) = delete;
};

#endif // DEMO_HPP
```

And the gripping conclusion - `Demo.cpp`:

```cpp
#include "vklearnin/vklearnin.hpp"
#include "Demo.hpp"

Demo::Demo() :
    vkl::Application()
{ }
```

I know, right? Poetry. Don't let your guard down though; `main.cpp` is a doozy as well.

```cpp
#include "Demo.hpp"

int main() {
    auto *demo = new Demo;
    demo->run();
    delete demo;

    return 0;
}
```

Silliness aside, that should be all that's required to run a proper test for this chunk of the infrastructure upon which we'll rely. Compile and run! The output you'll receive will be quite verbose, due to `#define VKL_MEMLOG` in `vklearnin.hpp`. When I run on my current setup, I see 126 allocations and deallocations, followed by the sucess banner. If you simply comment out the `#define VKL_MEMLOG` line, the output will be much more concise, but you'll be able to know at a glance that the `spdlog` library is properly initialized and that your memory management is tidy. What I see is this:

![[Pasted image 20221228214807.png]]

I'll take it.

## Engine and Timekeeper
The `Engine` class will be where we manage game logic, so for the time being it'll be only slightly more interesting than `Application`. But, we'll also populate `Timekeeper` to have time-related metrics from the get-go.

The declaration for `Engine` looks like this.

```cpp
#ifndef VKLEARNIN_ENGINE_ENGINE_HPP
#define VKLEARNIN_ENGINE_ENGINE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Application;

class Engine final {
public:
    void render_loop();

    void init();
    void shutdown();

    explicit Engine(Application &app);
    ~Engine();

    Engine() = delete;

    Engine(Engine &&) = delete;
    Engine(const Engine &) = delete;

    Engine & operator=(Engine &&) = delete;
    Engine & operator=(const Engine &) = delete;

private:
    Application &_application;
};

} // namespace vkl

#endif // VKLEARNIN_ENGINE_ENGINE_HPP
```

While the implementation will simply initialize our one member variable:

```cpp
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/engine/Engine.hpp"

#include "vklearnin/system/Application.hpp"

namespace vkl {

// =============================================================================
void Engine::render_loop() {

}

// =============================================================================
void Engine::init() {

}

// =============================================================================
void Engine::shutdown() {

}

// =============================================================================
Engine::Engine(Application &app) :
    _application { app }
{ }

Engine::~Engine() {

}

} // namespace vkl
```

In `Application.hpp`, add a forward declaration of `Engine` within the `vkl` namespace. Then add a private member variable of type `Engine *` to the class. Correspondingly, add a line to `Application`'s constructor that creates a new `Engine` instance and a `delete` to the destructor.

Let's add some currently meaningless function calls to `Application::Run()`, too. `Application.cpp` will look like this.

```cpp
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/Application.hpp"

#include "vklearnin/engine/Engine.hpp"

namespace vkl {

// =============================================================================
void Application::run() {
    _engine->init();
    _engine->render_loop();
    _engine->shutdown();
}

// =============================================================================
Application::Application() :
    _engine { new Engine(*this) }
{
    ConsoleLog::init();
}

Application::~Application() {
	delete _engine;
}

} // namespace vkl
```

This should compile and run with the same output as before, of course. Provided everything's in order, let's look to `Timekeeper`.

The first change will be to `system/pch.hpp`, which needs `chrono`. After that, `Timekeeper.hpp` will be the most interesting header thus far.

```cpp
#ifndef VKLEARNIN_TOOLS_TIMEKEEPER_HPP
#define VKLEARNIN_TOOLS_TIMEKEEPER_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Timekeeper {
public:
    using HRC = std::chrono::high_resolution_clock;
    using Microseconds = std::chrono::microseconds;

    static void update();

    static void frame_start();
    static void frame_end();

    static inline auto runtime()   { return 1e-6f * _runtime;   }
    static inline auto frametime() { return 1e-6f * _frametime; }

    static inline const auto now() { return HRC::now(); }

    Timekeeper() = delete;
    ~Timekeeper() = delete;
    
    Timekeeper(Timekeeper &&other) = delete;
    Timekeeper(const Timekeeper &other) = delete;
    
    Timekeeper & operator=(Timekeeper &&other) = delete;
    Timekeeper & operator=(const Timekeeper &other) = delete;

private:
    static HRC::time_point _app_start;
    static HRC::time_point _frame_start;

    static uint64_t _runtime;
    static uint64_t _frametime;
};

} // namespace vkl

#endif // VKLEARNIN_TOOLS_TIMEKEEPER_HPP
```

And the implementation:

```cpp
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/tools/Timekeeper.hpp"

namespace vkl {

Timekeeper::HRC::time_point Timekeeper::_app_start   = Timekeeper::HRC::now();
Timekeeper::HRC::time_point Timekeeper::_frame_start = Timekeeper::_app_start;

uint64_t Timekeeper::_runtime   = 0u;
uint64_t Timekeeper::_frametime = 0u;

void Timekeeper::frame_start() {
    _frame_start = HRC::now();
}

void Timekeeper::frame_end() {
    auto interval = HRC::now() - _frame_start;
    _frametime = std::chrono::duration_cast<Microseconds>(interval).count();
}

void Timekeeper::update() {
    auto interval = HRC::now() - _app_start;
    _runtime = std::chrono::duration_cast<Microseconds>(interval).count();
}

} // namespace vkl
```

## Testing, Round Two
It's inarguably early to talk about frame times, but let's just establish some more library-client code interaction as well as a touch more output as a sanity check. Add `Timekeeper.hpp` to `vklearnin.hpp` and let's turn to `Application::run()`. Add the following:

```cpp
void Application::run() {
    _engine->init();

    Timekeeper::frame_start();
        _engine->render_loop();
    Timekeeper::frame_end();
    CONSOLE_TRACE("Sixty frames might take: {}", Timekeeper::frametime());

    _engine->shutdown();
}
```

And in `Engine::render_loop()`, let's give `std::chrono` something to count.

```cpp
void Engine::render_loop() {
    using sixty_fps = std::chrono::duration<float, std::ratio<1, 60>>;
    std::this_thread::sleep_for(sixty_fps(60));
}
```

If you now compile and run the code, you should see a single trace that indicates something like one second was spent in `Engine::render_loop()`. For example:

![[Pasted image 20221228214912.png]]

Stellar. Let's move on to some Vulkan code, finally.