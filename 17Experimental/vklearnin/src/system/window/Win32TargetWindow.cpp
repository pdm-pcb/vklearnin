#ifdef VKL_WINDOWS

#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/window/Win32TargetWindow.hpp"

#include "vklearnin/system/GraphicsAPI.hpp"

namespace vkl {

::HWND   Win32TargetWindow::_window       = nullptr;
::LPCSTR Win32TargetWindow::_classname    = nullptr;
::LPCSTR Win32TargetWindow::_window_title = nullptr;
::HDC    Win32TargetWindow::_device       = nullptr;
::LPBYTE Win32TargetWindow::_raw_message  = nullptr;

Win32TargetWindow::ScreenPos Win32TargetWindow::_center;

vk::SurfaceKHR Win32TargetWindow::_surface { };

// =============================================================================
// Given that this is a single threaded project (for now...) there needs to be
// one function that drops in and checks on the OS in a non-blocking manner.
void Win32TargetWindow::message_loop() {
    ::MSG message { };
    while(::PeekMessageA(&message, _window, 0u, 0u, PM_REMOVE) != 0) {
        // Only needed for textual input, ala WM_CHAR. Keep it around for an
        // eventual dev terminal perhaps?
        // ::TranslateMessage(&message);
        ::DispatchMessageA(&message);
    }
}

// =============================================================================
::LRESULT Win32TargetWindow::_wndproc(::HWND window, ::UINT message,
                                      ::WPARAM wparam, ::LPARAM lparam)
{
    switch(message) {
        case WM_KEYDOWN: // Basic input handling
            EventBroker::emit<KeyPressEvent>(win32_to_vkl(wparam));
            if(wparam == VK_ESCAPE) {
                // Kick off the process of cleaning up after ourselves
                ::SendMessage(window, WM_CLOSE, wparam, lparam);
            }
            break;

        case WM_KEYUP:
            EventBroker::emit<KeyReleaseEvent>(win32_to_vkl(wparam));
            break;

        case WM_INPUT: {
            // Check message size
            ::UINT message_size;
            auto result = ::GetRawInputData(
                reinterpret_cast<::HRAWINPUT>(lparam),
                RID_INPUT,
                nullptr,
                &message_size,
                sizeof(::RAWINPUTHEADER)
            );

            if(result == (::UINT) -1) {
                CONSOLE_CRITICAL("Failed to get win32 raw input message");
                break;
            }

            // Get actual message
            result = ::GetRawInputData(
                reinterpret_cast<::HRAWINPUT>(lparam),
                RID_INPUT,
                _raw_message,
                &message_size,
                sizeof(::RAWINPUTHEADER)
            );
            if(result != message_size) {
                CONSOLE_CRITICAL(
                    "Win32 raw input expected {} bytes, got {} bytes instead",
                    message_size, result
                );
                break;
            }

            // Cast to useful type
            auto const *input = reinterpret_cast<::RAWINPUT *>(_raw_message);
            switch(input->header.dwType) {
                case RIM_TYPEMOUSE: {
                    auto const &mouse = input->data.mouse;

                    if(mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) {
                        EventBroker::emit<MouseButtonPressEvent>(
                            MOUSE_BUTTON_LEFT
                        );
                    }
                    else if(mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) {
                        EventBroker::emit<MouseButtonReleaseEvent>(
                            MOUSE_BUTTON_LEFT
                        );
                    }

                    if(mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
                        EventBroker::emit<MouseButtonPressEvent>(
                            MOUSE_BUTTON_RIGHT
                        );
                    }
                    else if(mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) {
                        EventBroker::emit<MouseButtonReleaseEvent>(
                            MOUSE_BUTTON_RIGHT
                        );
                    }

                    if(mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) {
                        EventBroker::emit<MouseButtonPressEvent>(
                            MOUSE_BUTTON_MIDDLE
                        );
                    }
                    else if(mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) {
                        EventBroker::emit<MouseButtonReleaseEvent>(
                            MOUSE_BUTTON_MIDDLE
                        );
                    }

                    if(mouse.lLastX != 0 || mouse.lLastY != 0) {
                        EventBroker::emit<MouseMoveEvent>(
                            mouse.lLastX,
                            mouse.lLastY
                        );
                    }

                    if(mouse.usButtonFlags & RI_MOUSE_WHEEL) {
                        EventBroker::emit<MouseScrollEvent>(
                            static_cast<short>(mouse.usButtonData),
                            0
                        );
                    }

                    if(mouse.usButtonFlags & RI_MOUSE_HWHEEL) {
                        EventBroker::emit<MouseScrollEvent>(
                            0,
                            static_cast<short>(mouse.usButtonData)
                        );
                    }
                    break;
                }
                default:
                    break;
            }

            break;
        }

        case WM_CREATE:
            _restrict_cursor();
            break;

        case WM_ACTIVATE:
            if(wparam == WA_ACTIVE || wparam == WA_CLICKACTIVE) {
                _restrict_cursor();
            }
            else if(wparam == WA_INACTIVE) {
                _release_cursor();
            }
            break;

        case WM_CLOSE: // The first message received in the shutdown process
            ::DestroyWindow(_window);
            ::UnregisterClassA(_classname, nullptr);
            return 0;

        case WM_DESTROY: // The second and final shutdown message
            ::PostQuitMessage(0);
            // Let Application know we're done for
            EventBroker::emit<WindowCloseEvent>();
            _release_cursor();
            return 0;

        default:
            break;
    }

    // Be sure to let Windows handle what hasn't already been handled
    return ::DefWindowProc(window, message, wparam, lparam);
}

// =============================================================================
void Win32TargetWindow::spawn_window(uint32_t const width,
                                     uint32_t const height,
                                     int32_t const  pos_x,
                                     int32_t const  pos_y)
{
    if(_window != nullptr) {
        CONSOLE_CRITICAL("Only one target window at a time is allowed.");
        return;
    }

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
        return;
    }

    _register_input();
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
    vk::Win32SurfaceCreateInfoKHR const surface_info {
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
void Win32TargetWindow::init() {
    // Making use of these constexprs again
    _classname    = ENGINE_NAME;
    _window_title = APP_NAME;

    // Set DPI awareness before querying for resolution
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

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
    auto const result = ::RegisterClassExA(&wcex);
    if(!SUCCEEDED(result)) {
        CONSOLE_CRITICAL("Could not register WNDCLASSEX with Windows.");
        return;
    }

    // Allocate the message structure for raw device input
    _raw_message  = new ::BYTE[64];
}

// =============================================================================
void Win32TargetWindow::shutdown() {
    delete[] _raw_message;
    _raw_message = nullptr;
}

// =============================================================================
void Win32TargetWindow::_register_input() {
    if(_raw_message == nullptr) {
        CONSOLE_CRITICAL(
            "Win32TargetWindow::_raw_message cannot be null. Did you forget to "
            "call Win32TargetWindow::init()?"
        );
        return;
    }

    ::RAWINPUTDEVICE devices[2];

    devices[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
    devices[0].usUsage     = HID_USAGE_GENERIC_KEYBOARD;
    devices[0].dwFlags     = 0; // RIDEV_NOLEGACY ?
    devices[0].hwndTarget  = _window;

    devices[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
    devices[1].usUsage     = HID_USAGE_GENERIC_MOUSE;
    devices[1].dwFlags     = RIDEV_NOLEGACY;
    devices[1].hwndTarget  = _window;

    auto result =
        ::RegisterRawInputDevices(devices, 2, sizeof(::RAWINPUTDEVICE));

    if(result == FALSE) {
        auto const error = ::GetLastError();
        auto const error_message = std::system_category().message(error);
        CONSOLE_CRITICAL(
            "Failed to register for raw win32 input with error: '{}'",
            error_message
        );
        return;
    }

    CONSOLE_TRACE("Registered for raw win32 input");
}

// =============================================================================
void Win32TargetWindow::_restrict_cursor() {
	::RECT client_area;
	::GetClientRect(_window, &client_area);
	::MapWindowPoints(
        _window,
        nullptr, // Convert window-relative coordinates to desktop coordinates
        reinterpret_cast<::POINT *>(&client_area),
        2
    );
    // Restrict the cursor to moving within the client area
	::ClipCursor(&client_area);

    // Run through all requests to show a cursor until there are none
    while(::ShowCursor(FALSE) >= 0);
}

// =============================================================================
void Win32TargetWindow::_release_cursor() {
    // Allow the cursor to travel outside the client space
    ::ClipCursor(nullptr);

    // Queue requests until there are some
    while(::ShowCursor(TRUE) < 0);
}

// =============================================================================
void Win32TargetWindow::_size_and_place() {
    CONSOLE_TRACE(
        "Window size: {}x{}, position: {}x{}",
        RenderConfig::window_width, RenderConfig::window_height,
        RenderConfig::window_pos_x, RenderConfig::window_pos_y
    );

    auto result = ::SetWindowPos(
        _window, nullptr,
        static_cast<int>(RenderConfig::window_pos_x),
        static_cast<int>(RenderConfig::window_pos_y),
        static_cast<int>(RenderConfig::window_width),
        static_cast<int>(RenderConfig::window_height),
        0
    );


    if(result == FALSE) {
        auto const error = ::GetLastError();
        auto const error_message = std::system_category().message(error);
        CONSOLE_CRITICAL(
            "Failed to set win32 window position with error: '{}'",
            error_message
        );
        return;
    }
}

} // namespace vkl

#endif // VKL_WINDOWS