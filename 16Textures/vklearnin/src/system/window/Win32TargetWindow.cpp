#ifdef VKL_WINDOWS

#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/window/Win32TargetWindow.hpp"

#include "vklearnin/system/GraphicsAPI.hpp"

namespace vkl {

bool     Win32TargetWindow::_carry_on     = true;
::HWND   Win32TargetWindow::_window       = nullptr;
::LPCSTR Win32TargetWindow::_classname    = nullptr;
::LPCSTR Win32TargetWindow::_window_title = nullptr;

Win32TargetWindow::CenterPos Win32TargetWindow::_center;

vk::SurfaceKHR Win32TargetWindow::_surface { };

// =============================================================================
// Given that this is a single threaded project (for now...) there needs to be
// one function that drops in and checks on the OS in a non-blocking manner.
// In this case, I'm also using the opportunity to keep Application abreast
// of whether or not TargetWindow wants to close.
bool Win32TargetWindow::message_loop() {
    ::MSG message { };
    while(::PeekMessageA(&message, _window, 0u, 0u, PM_REMOVE) != 0) {
        ::TranslateMessage(&message);
        ::DispatchMessageA(&message);
    }

    return _carry_on;
}

// =============================================================================
::LRESULT Win32TargetWindow::_wndproc(::HWND window, ::UINT message,
                                      ::WPARAM wparam, ::LPARAM lparam)
{
    switch(message) {
        case WM_KEYDOWN: // Basic input handling
            if(wparam == VK_ESCAPE) {
                // Kick off the process of cleaning up after ourselves
                ::SendMessage(window, WM_CLOSE, wparam, lparam);
            }
            break;

        case WM_CLOSE: // The first message received in the shutdown process
            ::DestroyWindow(_window);
            ::UnregisterClassA(_classname, nullptr);
            return 0;

        case WM_DESTROY: // The second and final shutdown message
            ::PostQuitMessage(0);
            // Let Application know we're done for
            _carry_on = false;
            return 0;

        default:
            break;
    }

    // Be sure to let Windows handle what hasn't already been handled
    return ::DefWindowProc(window, message, wparam, lparam);
}

// =============================================================================
void Win32TargetWindow::spawn_window(const uint32_t width,
                                     const uint32_t height,
                                     const int32_t  pos_x,
                                     const int32_t  pos_y)
{
    assert(_window == nullptr);

    _init();

    // If width and height aren't provided by Application, then just opt for
    // two-thirds of the available real estate
    if(width == 0u || height == 0u) {
        auto width_fraction  = static_cast<float>(RenderConfig::screen_width);
        auto height_fraction = static_cast<float>(RenderConfig::screen_height);
        width_fraction  *= 0.75f;
        height_fraction *= 0.75f;

        RenderConfig::window_width  = static_cast<uint32_t>(width_fraction);
        RenderConfig::window_height = static_cast<uint32_t>(height_fraction);
    }
    else {
        RenderConfig::window_width = width;
        RenderConfig::window_height = height;
    }

    RenderConfig::window_aspect =
        static_cast<float>(RenderConfig::window_width) /
        static_cast<float>(RenderConfig::window_height);

    // Determine the window's eventual position on screen
    auto half_width  = static_cast<int32_t>(RenderConfig::window_width)  / 2;
    auto half_height = static_cast<int32_t>(RenderConfig::window_height) / 2;
    if(pos_x == 0 || pos_y == 0) {
        RenderConfig::window_pos_x = _center.x - half_width;
        RenderConfig::window_pos_y = _center.y - half_height;
    }
    else {
        RenderConfig::window_pos_x = pos_x;
        RenderConfig::window_pos_y = pos_y;
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

    _size_and_place();

    CONSOLE_TRACE(
        "Created Win32 target window: {}x{} @ {:0.3f}",
        RenderConfig::window_width,
        RenderConfig::window_height,
        RenderConfig::window_aspect
    );
}

// =============================================================================
void Win32TargetWindow::create_surface() {
    // The details Vulkan cares about
    const vk::Win32SurfaceCreateInfoKHR surface_info {
        .hinstance = nullptr,
        .hwnd = _window,
    };

    // Create, check, assign
    auto result = GraphicsAPI::native().createWin32SurfaceKHR(surface_info);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Unable to create Win32 KHR surface: '{}'",
            to_string(result.result)
        );
    }
    _surface = result.value;

    CONSOLE_TRACE(
        "Created Vulkan surface {:#x}",
        reinterpret_cast<uint64_t>(::VkSurfaceKHR(_surface))
    );
}

// =============================================================================
void Win32TargetWindow::destroy_surface() {
    CONSOLE_TRACE(
        "Destroying Vulkan surface {:#x}",
        reinterpret_cast<uint64_t>(::VkSurfaceKHR(_surface))
    );
    GraphicsAPI::native().destroy(_surface);
}

// =============================================================================
void Win32TargetWindow::_init() {
    // Making use of these constexprs again
    _classname    = ENGINE_NAME;
    _window_title = APP_NAME;

    // Win32 does make it very easy to query the primary display's resolution,
    // but I am 100% sure this would break on even the most modest variation
    // on a standard setup. I'd like to try it with a laptop and HDMI out to
    // a TV and see what I get, for example.
    RenderConfig::screen_width  =
        static_cast<uint32_t>(::GetSystemMetrics(SM_CXSCREEN));
    RenderConfig::screen_height =
        static_cast<uint32_t>(::GetSystemMetrics(SM_CYSCREEN));

    // I'm choosing to center the window on the primary screen
    _center.x = static_cast<int32_t>(RenderConfig::screen_width)  / 2;
    _center.y = static_cast<int32_t>(RenderConfig::screen_height) / 2;

    // Now the win32 specific stuff
    ::WNDCLASSEXA wcex { };
    wcex.cbSize        = sizeof(::WNDCLASSEXA);
    // Redraw the entire window if the size changes, not just the new area
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = _wndproc;
    // Set the background of the window to black so it doesn't blind me
    wcex.hbrBackground = static_cast<::HBRUSH>(::GetStockObject(BLACK_BRUSH));
    wcex.lpszClassName = _classname;

    // Check to see if Windows wants us to use this class definition
    const ::HRESULT result = ::RegisterClassExA(&wcex);
    if(!SUCCEEDED(result)) {
        CONSOLE_CRITICAL("Could not register WNDCLASSEX with Windows.");
    }
}

// =============================================================================
void Win32TargetWindow::_size_and_place() {
    // The only noteworthy detail here is that, as it is currently set up,
    // win32 is not DPI-aware. That means that if you've got display scaling
    // enabled (Windows often enables it by default) the resolution you passed
    // in from Application won't reflect that true resolution on your monitor.
    // Instead, it'll be scaled proportionately. The above code default works
    // regardless of scaling, of course.

    CONSOLE_TRACE(
        "Window size: {}x{}, position: {}x{}",
        RenderConfig::window_width, RenderConfig::window_height,
        RenderConfig::window_pos_x, RenderConfig::window_pos_y
    );

    ::SetWindowPos(
        _window, nullptr,
        static_cast<int>(RenderConfig::window_pos_x),
        static_cast<int>(RenderConfig::window_pos_y),
        static_cast<int>(RenderConfig::window_width),
        static_cast<int>(RenderConfig::window_height),
        0
    );
}

} // namespace vkl

#endif // VKL_WINDOWS