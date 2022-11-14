## Organizational Touch-Up
Is it just me, or is the project’s `include/` getting a little messy? I’m going to add three folders to both `include/` and `src/`: `rendering/`, `system/`, and `tools/`. I’ll put `GraphicsInstance` `rendering/`. As for `Application` and `pch.hpp`, they'll live in `system/`. Finally, `MemTracker` and `VKDebugger` go in `tools/`. All of this entails updating a number of `#include` statements as well as some guards, but I prefer it this way. By all means, organize your code however you prefer. ;)

## `RenderConfig`
It's time to add another bit of helpfulness to `system/` - a `struct` called `RenderConfig`. It will contain the state of affairs that parts of the application will need to share with one another. In brief:

```cpp
#ifndef VKLEARNIN_SYSTEM_RENDERCONFIG_HPP
#define VKLEARNIN_SYSTEM_RENDERCONFIG_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct RenderConfig {
    static uint16_t screen_width;
    static uint16_t screen_height;
    static uint16_t screen_x_offset;
    static uint16_t screen_y_offset;

    static uint16_t window_width;
    static uint16_t window_height;
};

} // namespace vkl

#endif // VKLEARNIN_SYSTEM_RENDERCONFIG_HPP
```

And correspondingly, in a rightly named `.cpp` file:

```cpp
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/RenderConfig.hpp"

namespace vkl {

uint16_t RenderConfig::screen_width    = 0;
uint16_t RenderConfig::screen_height   = 0;
uint16_t RenderConfig::screen_x_offset = 0;
uint16_t RenderConfig::screen_y_offset = 0;

uint16_t RenderConfig::window_width  = 0;
uint16_t RenderConfig::window_height = 0;

} // namespace vkl
```

## Win32 Programming for Vulkan
Now let's build a window class, . There's going to be a whole lot of `static` going on, because win32 is old. But hey, it's snappy! I'm calling my class `Win32TargetWindow`.

Add these five public member functions:

```cpp
// Give the OS a moment to speak up
static bool message_loop();

// Setting up and shutting down the native window
static void init(const uint16_t width = 0u, const uint16_t height = 0u);
static void init_surface(const vk::Instance &instance);
static void shutdown(const vk::Instance &instance);

// For those who would like to know
inline static vk::SurfaceKHR & surface() { return _surface; }
```

Add the big six, but just delete them all; we don't want any actual instances of this class. Moving on to the private members, there will be four Windows specific handles, two facilitating how I want this to work, one for Vulkan, and two supporting functions.

```cpp
// Win32 specifics
static ::HWND   _window;
static ::LPCSTR _classname;
static ::LPCSTR _window_title;
static ::HDC    _device;

// Helps with centering the window on screen
static struct CenterPos {
	uint16_t x;
	uint16_t y;
} _center;

// A flag for when the user opts to close the program
static bool _running;

// Vulkan specifics
static vk::SurfaceKHR _surface;

// The way in for Windows
static ::LRESULT CALLBACK
_wndproc(::HWND window, ::UINT msg, ::WPARAM wparam, ::LPARAM lparam);

// Make it just right
static void _size_and_place(const uint16_t width, const uint16_t height,
							const uint16_t pos_x, const uint16_t pos_y);
```

With the skeleton roughed out, let's put some meat on its bones. There's no constructor, so let's go with `init()` to start with.

```cpp
void Win32TargetWindow::init(const uint16_t width, const uint16_t height) {
    // Making use of these constexprs again
    _classname    = ENGINE_NAME;
    _window_title = APP_NAME;

    // Win32 does make it very easy to query the primary display's resolution,
    // but I am 100% sure this would break on even the most modest variation
    // on a standard setup. I'd like to try it with a laptop and HDMI out to
    // a TV and see what I get, for example.
    RenderConfig::screen_width  =
        static_cast<uint16_t>(::GetSystemMetrics(SM_CXSCREEN));
    RenderConfig::screen_height =
        static_cast<uint16_t>(::GetSystemMetrics(SM_CYSCREEN));

    // I'm choosing to center the window on the primary screen
    _center.x = (RenderConfig::screen_width / 2);
    _center.y = (RenderConfig::screen_height / 2);

    // If width and height aren't provided by Application, then just opt for
    // 75% of the available real estate
    if(width == 0u || height == 0u) {
        RenderConfig::window_width  = 
            static_cast<uint16_t>(RenderConfig::screen_width * 0.75f);
        RenderConfig::window_height = 
            static_cast<uint16_t>(RenderConfig::screen_height * 0.75f);
    }
    else {
        RenderConfig::window_width = width;
        RenderConfig::window_height = height;
    }

    // Determine the window's eventual position on screen
    uint16_t pos_x = _center.x - (RenderConfig::window_width  / 2);
    uint16_t pos_y = _center.y - (RenderConfig::window_height / 2);

    // Now for the actual win32 code.
    ::WNDCLASSEXA wcex { };
    wcex.cbSize        = sizeof(::WNDCLASSEXA);
    // Redraw the entire window if the size changes, not just the new area
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = _wndproc;
    // Set the background of the window to black so it doesn't blind me
    wcex.hbrBackground = static_cast<::HBRUSH>(::GetStockObject(BLACK_BRUSH));
    wcex.lpszClassName = _classname;

    // Check to see if Windows wants us to use this class definition
    ::HRESULT result = ::RegisterClassExA(&wcex);
    if(!SUCCEEDED(result)) {
        CONSOLE_CRITICAL("Could not register WNDCLASSEX with Windows.");
    }

    // Create!
    _window = ::CreateWindowExA(
        0u,                     // extended style
        _classname,             // win32 class name
        _window_title,          // win32 window title
        WS_POPUP | WS_VISIBLE,  // No decorations, visible by default
        CW_USEDEFAULT,          // x location
        CW_USEDEFAULT,          // y location
        static_cast<int>(RenderConfig::window_width),   // width
        static_cast<int>(RenderConfig::window_height),  // height
        nullptr,    // parent window handle
        nullptr,    // menu handle
        nullptr,    // instance handle
        nullptr     // pointer to lParam; retrieved via WM_CREATE
    );

    if(_window == nullptr) {
        CONSOLE_CRITICAL("Unable to create win32 window.");
    }

    _size_and_place(RenderConfig::window_width, RenderConfig::window_height,
                    pos_x, pos_y);

    CONSOLE_TRACE(
        "Created Win32 target window: {}x{}",
        RenderConfig::window_width,
        RenderConfig::window_height
    );

    // And, go.
    _running = true;
}
```

Add a private member function called `_size_and_place()`.

```cpp
void Win32TargetWindow::_size_and_place(const uint16_t width,
                                        const uint16_t height,
                                        const uint16_t pos_x,
                                        const uint16_t pos_y)
{
    // The only noteworthy detail here is that, as it is currently set up,
    // win32 is not DPI-aware. That means that if you've got display scaling
    // enabled (Windows often enables it by default) the resolution you passed
    // in from Application won't reflect that true resolution on your monitor.
    // Instead, it'll be scaled proportionately. The above 75% default works
    // regardless of scaling, of course.

    CONSOLE_TRACE("Window size: {}x{}, position: {}x{}",
                   width, height, pos_x, pos_y);

    ::SetWindowPos(
        _window, nullptr,
        static_cast<int>(pos_x),
        static_cast<int>(pos_y),
        static_cast<int>(width),
        static_cast<int>(height),
        0
    );

    _center.x = pos_x + (width / 2);
    _center.x = pos_x + (width / 2);
}
```

Not too bad, not too bad! Let's give `message_loop()` a look.

```cpp
// Given that this is a single threaded project (for now...) there needs to be
// one function that drops in and checks on the OS in a non-blocking manner.
// In this case, I'm also using the opportunity to keep Application abreast
// of whether or not TargetWindow wants to close.
bool Win32TargetWindow::message_loop() {
    ::MSG message { };
    while(::PeekMessageA(&message, _window, 0u, 0u, PM_REMOVE)) {
        ::TranslateMessage(&message);
        ::DispatchMessageA(&message);
    }

    return _running;
}
```

`PeekMessage()` is the handy, non-blocking way to interact with the OS. We're really only interested in a few messages though, so `_wndproc()` is comparatively small.

```cpp
// There are a couple of methods to avoid needing static everything for a win32
// window, but I opted for simplicity this time around.
::LRESULT Win32TargetWindow::_wndproc(::HWND window, ::UINT message,
                                      ::WPARAM wparam, ::LPARAM lparam)
{
    switch(message) {
        case WM_KEYDOWN: // The most rudimentary input handling
            if(wparam == VK_ESCAPE) {
                _running = false;
            }
            // This is how we kick off the process of cleaning up after
            // ourselves
            ::SendMessage(window, WM_CLOSE, wparam, lparam);
            break;

        case WM_SIZE: // Keep the central records up to date
            RenderConfig::window_width  = LOWORD(lparam);
            RenderConfig::window_height = HIWORD(lparam);
            break;

        case WM_CLOSE: // The first message received in the shutdown process
            ::DestroyWindow(_window);
            ::UnregisterClassA(_classname, 0);
            return 0;

        case WM_DESTROY: // The second and final shutdown message
            ::PostQuitMessage(0);
            return 0;

        default: break;
    }

    return ::DefWindowProc(window, message, wparam, lparam);
}
```

And that is genuinely all we need for a functional Vulkan window with win32 as the backend. At present count, I get 205 lines. I'll take it.

## Vulkan Surfaces
The concept of a surface is pretty straight forward - it's simply an area of the display Vulkan knows how to draw to. Required details for drawing include color space and image format, among others.

`Win32TargetWindow::init_surface()` and `shutdown()` are both short and sweet.

```cpp
void Win32TargetWindow::init_surface(const vk::Instance &instance) {
    // I'm asking the instance to destroy the existing surface, if  there is
    // one, because it'll smooth out resizing the window later on.
    instance.destroy(_surface);

    vk::Win32SurfaceCreateInfoKHR surface_info {
        .hinstance = nullptr,
        .hwnd = _window,
    };

    // Create, check, assign
    auto result = instance.createWin32SurfaceKHR(surface_info);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to create Win32 KHR surface");
    }
    _surface = result.value;

    CONSOLE_TRACE(
        "Created Vulkan surface {}",
        reinterpret_cast<uint64_t>(::VkSurfaceKHR(_surface))
    );
}

void Win32TargetWindow::shutdown(const vk::Instance &instance) {
    CONSOLE_TRACE(
        "Destroying Vulkan surface {}",
        reinterpret_cast<uint64_t>(::VkSurfaceKHR(_surface))
    );
    instance.destroy(_surface);
}
```

## Back to Application
With the window squared away, let's return to `Application` and make use of this stuff. First, add a private member variable, `bool Application::_running` and initialize it to true in the constructor (the `TargetWindow` defaults to false, but we want `Application` to assume all is well).

In `Application.cpp`, include `Win32TargetWindow.hpp` from wherever you've got it stored.

If you aren't planning on doing any cross-compiling for Linux, you can skip this, but mind the naming conventions as we progress. Add the following hadny-dandy preprocessor block so we can unify the two types of `TargetWindow`.

```cpp
#if defined(__linux__)
    using TargetWindow = XCBTargetWindow;
#elif defined(_WIN32)
    using TargetWindow = Win32TargetWindow;
#endif
```

Next up, let's alter `Application::init()` to fire us up a window.

```cpp
void Application::init() {
    _graphics_instance = new GraphicsInstance;
    _graphics_instance->init();

    TargetWindow::init();
    TargetWindow::init_surface(_graphics_instance->native());
}
```

If you want to set a specific size for your window, feel free to do so via `Win32TargetWindow::init()`'s optional parameters. Of course anything that looks like a `new` (even just a call to `init()`) is likely to need it's other half, so the `Application` destructor now looks like this:

```cpp
Application::~Application() {
    TargetWindow::shutdown(_graphics_instance->native());
    delete _graphics_instance;
}
```

I like to shut down/destroy whatever I've got in the reverse order I set it up. This is how C++ does things internally, so I like to reflect that principle in my own design decisions.

Lastly, we finally get to give `Application::run()` something to do. =) It ain't much, but we're gaining momentum now.

```cpp
void Application::run() {
    while(_running) {
        _running = TargetWindow::message_loop();
    }
}
```

And that's it! If your code compiles and links, you should be presented with a blank, black window with no title bar or Windows controls, smack center in your primary screen. If so, go pour yourself a beverage and celebrate. If not, give the working code a looksee and compare it to yours.

This window will persist until you hit escape, at which point it will close, `VKDebugger` should not have anything to complain about, and `MemTracker` should congratulate you as well.

Next chapter will return to platform independence as well as the Vulkan notion of command queues.