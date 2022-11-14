#ifdef _WIN32

#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/Win32TargetWindow.hpp"

namespace vkl {

::HWND   Win32TargetWindow::_window       = nullptr;
::LPCSTR Win32TargetWindow::_classname    = nullptr;
::LPCSTR Win32TargetWindow::_window_title = nullptr;
::HDC    Win32TargetWindow::_device       = nullptr;

Win32TargetWindow::CenterPos Win32TargetWindow::_center { .x = 0u, .y = 0u };

vk::SurfaceKHR Win32TargetWindow::_surface = nullptr;

bool Win32TargetWindow::_running = false;

//==============================================================================
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

//==============================================================================
// There are a couple of methods to avoid needing static everything for a win32
// window, but I opted for simplicity this time around.
::LRESULT Win32TargetWindow::_wndproc(::HWND window, ::UINT message,
                                      ::WPARAM wparam, ::LPARAM lparam)
{
    switch(message) {
        case WM_KEYDOWN: // The most rudimentary input handling
            if(wparam == VK_ESCAPE) {
                _running = false;
                // This is how we kick off the process of cleaning up after
                // ourselves
                ::SendMessage(window, WM_CLOSE, wparam, lparam);
            }
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

//==============================================================================
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

//==============================================================================
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

//==============================================================================
void Win32TargetWindow::shutdown(const vk::Instance &instance) {
    CONSOLE_TRACE(
        "Destroying Vulkan surface {}",
        reinterpret_cast<uint64_t>(::VkSurfaceKHR(_surface))
    );
    instance.destroy(_surface);
}

//==============================================================================
void Win32TargetWindow::_size_and_place(const uint16_t width,
                                        const uint16_t height,
                                        const uint16_t pos_x,
                                        const uint16_t pos_y)
{
    // The only noteworthy detail here is that, as it is currently set up,
    // win32 is not DPI-aware. That means that if you've got display scaling
    // enabled (Windows often enables it by default) the resolution you passed
    // in from Application won't reflect that true resolution on your monitor.
    // Instead, it'll be scaled proportionately. The above code default works
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

} // namespace vkl

#endif // _WIN32