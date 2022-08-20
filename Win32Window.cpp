#include "common.hpp"
#include "Win32Window.hpp"

#include "Instance.hpp"
#include "RenderLoop.hpp"

bool Win32Window::message_loop(RenderLoop &render_loop) {
    ::MSG message { };
    while(::PeekMessageA(&message, _hwindow, 0u, 0u, PM_REMOVE)) {
        ::TranslateMessage(&message);
        ::DispatchMessageA(&message);
    }
    
    render_loop.resized(_resized);
    return _running;
}

::LRESULT CALLBACK Win32Window::_message_dispatch(::HWND window,
                                                  ::UINT message,
                                                  ::WPARAM wparam,
                                                  ::LPARAM lparam)
{
    Win32Window *_this = nullptr;

    if(message == WM_NCCREATE) {
        ::LPCREATESTRUCT lpstruct = reinterpret_cast<::LPCREATESTRUCT>(lparam);
        _this = static_cast<Win32Window *>(lpstruct->lpCreateParams);

        ::SetWindowLongPtrA(
            window,
            GWLP_USERDATA,
            reinterpret_cast<::LONG_PTR>(_this)
        );

        _this->_hwindow = window;
    }
    else {
        _this = reinterpret_cast<Win32Window *>(
            ::GetWindowLongPtrA(window, GWLP_USERDATA)
        );
    }

    if(_this) {
        return _this->_message_handler(window, message, wparam, lparam);
    }
    else {
        return ::DefWindowProcA(window, message, wparam, lparam);
    }
}

::LRESULT Win32Window::_message_handler(::HWND window, ::UINT message,
                                        ::WPARAM wparam, ::LPARAM lparam)
{
    switch(message) {
        case WM_KEYUP:
            switch(wparam) {
                case VK_ESCAPE:
                    ::SendMessageA(_hwindow, WM_CLOSE, 0u, 0);
                    break;
                case 0x41: // 'a'
                    if(_extent.width == 1280u) {
                        break;
                    }
                    CONSOLE_TRACE("bumping up resolution");
                    _build_window(1280u, 1024u);
                    break;
                case 0x5A: // 'z'
                    if(_extent.width == 1024u) {
                        break;
                    }
                    CONSOLE_TRACE("toning down resolution");
                    _build_window(1024u, 768u);
                    break;
                case 0x46: { // 'f'
                    uint32_t display_x =
                        static_cast<uint32_t>(::GetSystemMetrics(SM_CXSCREEN));
                    uint32_t display_y =
                        static_cast<uint32_t>(::GetSystemMetrics(SM_CYSCREEN));
                    if(_extent.width == display_x) {
                        CONSOLE_TRACE("Exiting fullscreen");
                        _build_window(1024u, 768u);
                    }
                    else {
                        CONSOLE_TRACE("FULLSCREEN YOU SAY?!?!");
                        _build_window(display_x, display_y);
                    }

                    break;
                }
            }
            break;

        case WM_SIZE:
            _resized = true;
            break;

        case WM_CLOSE:
            ::DestroyWindow(_hwindow);
            ::UnregisterClassA(ENGINE_NAME, 0);
            return 0;

        case WM_DESTROY:
            _running = false;
            ::PostQuitMessage(0);
            return 0;

        default: break;
    }

    return ::DefWindowProc(window, message, wparam, lparam);
}

void Win32Window::init_window() {
    CONSOLE_INFO("");
    ::WNDCLASSEXA wcex { };
    wcex.cbSize = sizeof(::WNDCLASSEXA);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = _message_dispatch;
    wcex.hInstance = _hinstance;
    wcex.hIcon = ::LoadIcon(_hinstance, IDI_APPLICATION);
    wcex.hIconSm = ::LoadIcon(_hinstance, IDI_APPLICATION);
    wcex.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = static_cast<::HBRUSH>(::GetStockObject(BLACK_BRUSH));
    wcex.lpszClassName = APPLICATION_NAME;

    ::HRESULT result = ::RegisterClassEx(&wcex);
    if(!SUCCEEDED(result)) {
        CONSOLE_CRITICAL("Could not register WNDCLASSEX with Windows.");
    }

    _hwindow = ::CreateWindowExA(
        0,
        APPLICATION_NAME,
        APPLICATION_NAME,
        WS_POPUP | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        static_cast<int>(_extent.width),
        static_cast<int>(_extent.height),
        nullptr, nullptr, 0, this
    );

    if(_hwindow == nullptr) {
        CONSOLE_CRITICAL("Unable to create win32 window.");
    }

    _build_window(_extent.width, _extent.height);
    _running = true;
}

void Win32Window::init_surface() {
    CONSOLE_INFO("");

    ::VkWin32SurfaceCreateInfoKHR surface_info { };
    surface_info.sType = ::VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surface_info.pNext = nullptr;
    surface_info.flags = 0u;
    surface_info.hinstance = _hinstance;
    surface_info.hwnd = _hwindow;

    ::VkResult result = ::vkCreateWin32SurfaceKHR(
        _instance,
        &surface_info,
        nullptr,
        &_surface
    );

    if(result != VK_SUCCESS) {
        CONSOLE_CRITICAL("Unable to create Vulkan surface");
    }
}

void Win32Window::_build_window(const uint32_t x_res, const uint32_t y_res) {
    _extent = { x_res, y_res };
    int display_x = ::GetSystemMetrics(SM_CXSCREEN);
    int display_y = ::GetSystemMetrics(SM_CYSCREEN);

    int pos_x = display_x / 2 - (_extent.width  / 2);
    int pos_y = display_y / 2 - (_extent.height / 2);

    ::SetWindowPos(
        _hwindow, nullptr,
        pos_x, pos_y,
        static_cast<int>(_extent.width),
        static_cast<int>(_extent.height),
        0
    );
}

Win32Window::Win32Window(const uint32_t x_res, const uint32_t y_res,
                         const int32_t x_offset, const int32_t y_offset,
                         const ::VkInstance &instance) :
    _hinstance { nullptr  },
    _hwindow   { nullptr  },
    _surface   { nullptr  },
    _running   { false    },
    _resized   { false    },
    _offset    { x_offset, y_offset },
    _extent    { x_res, y_res },
    _instance  { instance }
{
    CONSOLE_INFO("");
}

Win32Window::~Win32Window() {
    CONSOLE_INFO("");

    ::vkDestroySurfaceKHR(_instance, _surface, nullptr);
}