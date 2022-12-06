#ifdef _WIN32

#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/Win32TargetWindow.hpp"

#include "vklearnin/rendering/GraphicsInstance.hpp"
#include "vklearnin/system/events/EventBroker.hpp"

namespace vkl {

::HWND   Win32TargetWindow::_window       = nullptr;
::LPCSTR Win32TargetWindow::_classname    = nullptr;
::LPCSTR Win32TargetWindow::_window_title = nullptr;
::HDC    Win32TargetWindow::_device       = nullptr;
::LPBYTE Win32TargetWindow::_raw_message  = new ::BYTE[64];

Win32TargetWindow::CenterPos Win32TargetWindow::_center { .x = 0u, .y = 0u };

vk::SurfaceKHR Win32TargetWindow::_surface = nullptr;

// =============================================================================
// Given that this is a single threaded project (for now...) there needs to be
// one function that drops in and checks on the OS in a non-blocking manner.
// In this case, I'm also using the opportunity to keep Application abreast
// of whether or not TargetWindow wants to close.
void Win32TargetWindow::message_loop() {
    ::MSG message { };
    while(::PeekMessageA(&message, _window, 0u, 0u, PM_REMOVE)) {
        ::TranslateMessage(&message);
        ::DispatchMessageA(&message);
    }
}

// =============================================================================
// There are a couple of methods to avoid needing static everything for a win32
// window, but I opted for simplicity this time around.
::LRESULT Win32TargetWindow::_wndproc(::HWND window, ::UINT message,
                                      ::WPARAM wparam, ::LPARAM lparam)
{
    switch(message) {
        case WM_KEYDOWN: // The most rudimentary input handling
            if(wparam == VK_ESCAPE) {
                // This is how we kick off the process of cleaning up after
                // ourselves
                EventBroker::emit<WindowCloseEvent>();
                ::SendMessage(window, WM_CLOSE, wparam, lparam);
            }
            break;

        case WM_SYSKEYUP:
        // courtesey jonrhythmic:
        // https://www.reddit.com/r/learnprogramming/comments/nqrt4o/comment/h0d1te9/
            if((HIWORD(lparam) & KF_ALTDOWN) && (LOWORD(wparam) == VK_RETURN)) {
                static uint16_t old_width  = 0u;
                static uint16_t old_height = 0u;

                if(RenderConfig::window_width == RenderConfig::screen_width &&
                   RenderConfig::window_height == RenderConfig::screen_height)
                {
                    uint16_t pos_x = _center.x - (old_width  / 2);
                    uint16_t pos_y = _center.y - (old_height / 2);
                    _size_and_place(old_width, old_height, pos_x, pos_y);
                }
                else {
                    old_width  = RenderConfig::window_width;
                    old_height = RenderConfig::window_height;
                    _size_and_place(RenderConfig::screen_width,
                                    RenderConfig::screen_height,
                                    0u, 0u);
                }
            }
            break;

        case WM_SIZE: // Keep the central records up to date
            RenderConfig::window_width  = LOWORD(lparam);
            RenderConfig::window_height = HIWORD(lparam);
            RenderConfig::aspect_ratio =
                RenderConfig::window_width /
                static_cast<float>(RenderConfig::window_height);
            CONSOLE_TRACE(
                "{}x{}, {}",
                RenderConfig::window_width,
                RenderConfig::window_height,
                RenderConfig::aspect_ratio
            );
            break;

        case WM_CLOSE: // The first message received in the shutdown process
            ::DestroyWindow(_window);
            ::UnregisterClassA(_classname, 0);
            return 0;

        case WM_DESTROY: // The second and final shutdown message
            ::PostQuitMessage(0);
            return 0;

        case WM_INPUT: {
            // Check message size
            ::UINT dwSize;
            ::GetRawInputData(
                (::HRAWINPUT) lparam,
                RID_INPUT,
                nullptr,
                &dwSize,
                sizeof(::RAWINPUTHEADER)
            );

            // Get actual message
            ::GetRawInputData(
                (::HRAWINPUT) lparam,
                RID_INPUT,
                _raw_message,
                &dwSize,
                sizeof(::RAWINPUTHEADER)
            );

            // ------------------------------------------------------------------
            // Thanks to Stefan Reinalter for much of the following code
            // https://blog.molecular-matters.com/2011/09/05/properly-handling-keyboard-input/
            //
            ::RAWINPUT *input = (::RAWINPUT *)_raw_message;

            switch(input->header.dwType) {
                case RIM_TYPEKEYBOARD:
                {                
                    ::UINT vkey  = input->data.keyboard.VKey;
                    ::UINT code  = input->data.keyboard.MakeCode;
                    ::UINT flags = input->data.keyboard.Flags;                    
                    
                    // discard "fake keys" which are part of an escaped sequence
                    if(vkey == 255) break;

                    // correct left-hand / right-hand SHIFT
                    else if(vkey == VK_SHIFT) {
                        vkey = MapVirtualKey(code, MAPVK_VSC_TO_VK_EX);
                    }

                    // correct PAUSE/BREAK and NUM LOCK silliness, and set
                    // the extended bit
                    else if(vkey == VK_NUMLOCK) {
                        code = (MapVirtualKey(vkey, MAPVK_VK_TO_VSC) | 0x100);
                    }

                    // exit condition
                    else if(vkey == VK_ESCAPE) {
                        EventBroker::emit<WindowCloseEvent>();
                        ::SendMessage(window, WM_CLOSE, wparam, lparam);
                        return 0;
                    }

                    // e0 and e1 are escape sequences used for certain special
                    // keys, such as PRINT and PAUSE/BREAK.
                    // see http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
                    const bool isE0 = ((flags & RI_KEY_E0) != 0);
                    const bool isE1 = ((flags & RI_KEY_E1) != 0);
                    
                    if(isE1) {
                        // for escaped sequences, turn the virtual key into the
                        // correct scan code using MapVirtualKey.
                        // however, MapVirtualKey is unable to map VK_PAUSE
                        // (this is a known bug), hence we map that by hand.
                        if(vkey == VK_PAUSE) {
                            code = 0x45;
                        }
                        else {
                            code = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
                        }
                    }

                    switch(vkey) {
                        // right-hand CONTROL and ALT have their e0 bit set
                        case VK_CONTROL:
                            if(isE0) vkey = KB_RCTRL;
                            else     vkey = KB_LCTRL;
                            break;
                        
                        case VK_MENU:
                            if(isE0) vkey = KB_RALT;
                            else     vkey = KB_LALT;
                            break;
                        
                        // NUMPAD ENTER has its e0 bit set
                        case VK_RETURN: if(isE0) vkey = KB_NP_ENTER; break;
                        
                        // the standard INSERT, DELETE, HOME, END, PRIOR and
                        // NEXT keys will always have their e0 bit set, but the
                        // corresponding keys on the NUMPAD will not.
                        case VK_INSERT: if(!isE0) vkey = KB_NP_0;       break;
                        case VK_DELETE: if(!isE0) vkey = KB_NP_DECIMAL; break;
                        case VK_HOME:   if(!isE0) vkey = KB_NP_7;       break;
                        case VK_END:    if(!isE0) vkey = KB_NP_1;       break;
                        case VK_PRIOR:  if(!isE0) vkey = KB_NP_9;       break;
                        case VK_NEXT:   if(!isE0) vkey = KB_NP_3;       break;
                        
                        // the standard arrow keys will always have their e0 bit
                        // set, but the corresponding keys on the NUMPAD will
                        // not.
                        case VK_LEFT:  if(!isE0) vkey = KB_NP_4; break;
                        case VK_RIGHT: if(!isE0) vkey = KB_NP_6; break;
                        case VK_UP:    if(!isE0) vkey = KB_NP_8; break;
                        case VK_DOWN:  if(!isE0) vkey = KB_NP_2; break;
                        
                        // NUMPAD 5 doesn't have its e0 bit set
                        case VK_CLEAR: if(!isE0) vkey = KB_NP_5; break;
                    }

                    // a key can either produce a "make" or "break" scancode.
                    // this is used to differentiate between down-presses and
                    // releases
                    // see http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
                    const bool was_release = ((flags & RI_KEY_BREAK) != 0);

                    if(was_release) {
                        EventBroker::emit<KeyReleaseEvent>(win32_to_vkl(vkey));
                    }
                    else {
                        EventBroker::emit<KeyPressEvent>(win32_to_vkl(vkey));
                    }

                    break;
                }
                case RIM_TYPEMOUSE:
                {
                    ::SetCursorPos(_center.x, _center.y);
                    ::SetCursor(nullptr);

                    ::RAWMOUSE mouse = input->data.mouse;

                    // if(mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) {
                    //     MouseButtonPressedEvent event(MOUSE_BUTTON_LEFT);
                    //     publish(EventType::MouseButtonPressed, event);
                    // }
                    // else if(mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) {
                    //     MouseButtonReleasedEvent event(MOUSE_BUTTON_LEFT);
                    //     publish(EventType::MouseButtonPressed, event);
                    // }
                    // else if(mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
                    //     MouseButtonPressedEvent event(MOUSE_BUTTON_RIGHT);
                    //     publish(EventType::MouseButtonReleased, event);
                    // }
                    // else if(mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) {
                    //     MouseButtonReleasedEvent event(MOUSE_BUTTON_RIGHT);
                    //     publish(EventType::MouseButtonReleased, event);
                    // }
                    // else if(mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) {
                    //     MouseButtonPressedEvent event(MOUSE_BUTTON_MIDDLE);
                    //     publish(EventType::MouseButtonPressed, event);
                    // }
                    // else if(mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) {
                    //     MouseButtonReleasedEvent event(MOUSE_BUTTON_MIDDLE);
                    //     publish(EventType::MouseButtonReleased, event);
                    // }
                    // else if(mouse.usButtonFlags & RI_MOUSE_WHEEL) {
                    //     MouseScrolledEvent event(0, mouse.usButtonData);
                    //     publish(EventType::MouseScrolled, event);
                    // }
                    // else if(mouse.usButtonFlags & RI_MOUSE_HWHEEL) {
                    //     MouseScrolledEvent event(mouse.usButtonData, 0);
                    //     publish(EventType::MouseScrolled, event);
                    // }

                    if(mouse.lLastX != 0 || mouse.lLastY != 0) {
                        EventBroker::emit<MouseMoveEvent>(
                            mouse.lLastX,
                            mouse.lLastY
                        );
                    }

                    break;
                }
                break;
            }
            break;
        }

        default: break;
    }

    return ::DefWindowProc(window, message, wparam, lparam);
}

// =============================================================================
void Win32TargetWindow::spawn_window(const uint16_t width,
                                     const uint16_t height)
{
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

    _register_input();

    _size_and_place(RenderConfig::window_width, RenderConfig::window_height,
                    pos_x, pos_y);

    CONSOLE_TRACE(
        "Created Win32 target window: {}x{}",
        RenderConfig::window_width,
        RenderConfig::window_height
    );
}

// =============================================================================
void Win32TargetWindow::create_surface() {
    vk::Win32SurfaceCreateInfoKHR surface_info {
        .hinstance = nullptr,
        .hwnd = _window,
    };

    // Create, check, assign
    const auto &instance = GraphicsInstance::native();
    auto result = instance.createWin32SurfaceKHR(surface_info);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to create Win32 KHR surface");
    }
    _surface = result.value;

    CONSOLE_TRACE(
        "Created Vulkan surface {:#x}",
        reinterpret_cast<uint64_t>(::VkSurfaceKHR(_surface))
    );
}

// =============================================================================
void Win32TargetWindow::destroy_surface() {
    const auto &instance = GraphicsInstance::native();
    CONSOLE_TRACE(
        "Destroying Vulkan surface {:#x}",
        reinterpret_cast<uint64_t>(::VkSurfaceKHR(_surface))
    );
    instance.destroy(_surface);
}

// =============================================================================
void Win32TargetWindow::shutdown() {
    destroy_surface();
    delete[] _raw_message;
}

// =============================================================================
void Win32TargetWindow::_register_input() {
    ::RAWINPUTDEVICE devices[2];

    devices[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
    devices[0].usUsage     = HID_USAGE_GENERIC_KEYBOARD;
    devices[0].dwFlags     = 0; // RIDEV_NOLEGACY ?
    devices[0].hwndTarget  = _window;

    devices[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
    devices[1].usUsage     = HID_USAGE_GENERIC_MOUSE;
    devices[1].dwFlags     = RIDEV_NOLEGACY;
    devices[1].hwndTarget  = _window;

    if(!::RegisterRawInputDevices(devices, 2, sizeof(::RAWINPUTDEVICE))) {
        ::MessageBox(
            nullptr, "Could not register for raw HID input:",
            "Error", MB_OK    
        );
    }
    else {
        CONSOLE_TRACE("Registered for raw win32 input");
    }
}

// =============================================================================
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