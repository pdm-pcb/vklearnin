#ifdef WIN32

#include "vklearnin/common.hpp"
#include "vklearnin/Platform/Win32/Win32Window.hpp"

#include "vklearnin/Instance.hpp"
#include "vklearnin/RenderLoop.hpp"

//==============================================================================
bool Win32Window::message_loop() {
    ::MSG message { };
    while(::PeekMessageA(&message, _hwindow, 0u, 0u, PM_REMOVE)) {
        ::TranslateMessage(&message);
        ::DispatchMessageA(&message);
    }

    return _running;
}

//==============================================================================
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

//==============================================================================
::LRESULT Win32Window::_message_handler(::HWND window, ::UINT message,
                                        ::WPARAM wparam, ::LPARAM lparam)
{
    switch(message) {
        case WM_KEYDOWN:
            if(wparam == VK_ESCAPE) {
                ::SendMessageA(_hwindow, WM_CLOSE, 0u, 0);
            }
            break;

        case WM_SYSKEYUP:
        // courtesey jonrhythmic:
        // https://www.reddit.com/r/learnprogramming/comments/nqrt4o/comment/h0d1te9/
            if(HIWORD(lparam) && KF_ALTDOWN) {
                if(LOWORD(wparam) == VK_RETURN) {
                    static uint32_t old_width  = 0u;
                    static uint32_t old_height = 0u;

                    if(_width  == _screen_width && _height == _screen_height) {
                        _size_and_center(old_width, old_height);
                    }
                    else {
                        old_width  = _width;
                        old_height = _height;
                        _size_and_center(_screen_width, _screen_height);
                    }
                }
            }
            break;

        case WM_SIZE:
            _width = LOWORD(lparam);
            _height = HIWORD(lparam);
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

//==============================================================================
void Win32Window::init_window() {
    CONSOLE_INFO("");

    ::WNDCLASSEXA wcex { };
    wcex.cbSize        = sizeof(::WNDCLASSEXA);
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = _message_dispatch;
    wcex.hInstance     = _hinstance;
    wcex.hIcon         = ::LoadIcon(_hinstance, IDI_APPLICATION);
    wcex.hIconSm       = ::LoadIcon(_hinstance, IDI_APPLICATION);
    wcex.hCursor       = ::LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = static_cast<::HBRUSH>(::GetStockObject(BLACK_BRUSH));
    wcex.lpszClassName = APP_NAME;

    ::HRESULT result = ::RegisterClassEx(&wcex);
    if(!SUCCEEDED(result)) {
        CONSOLE_ERROR("Could not register WNDCLASSEX with Windows.");
    }

    _hwindow = ::CreateWindowExA(
        0u,
        APP_NAME,
        APP_NAME,
        WS_POPUP | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        static_cast<int>(_width),
        static_cast<int>(_height),
        nullptr, nullptr, nullptr,
        this
    );

    if(_hwindow == nullptr) {
        CONSOLE_ERROR("Unable to create win32 window.");
    }

    _size_and_center(_width, _height);
    _running = true;
}

//==============================================================================
void Win32Window::init_surface() {
    CONSOLE_INFO("");

    _instance.destroy(_surface);

    vk::Win32SurfaceCreateInfoKHR surface_info {
        .hinstance = _hinstance,
        .hwnd = _hwindow,
    };

    _surface = _instance.createWin32SurfaceKHR(surface_info);
}

//==============================================================================
void Win32Window::_size_and_center(const uint32_t width, const uint32_t height)
{
    int pos_x = _screen_width  / 2 - (width  / 2);
    int pos_y = _screen_height / 2 - (height / 2);

    ::SetWindowPos(
        _hwindow, nullptr,
        pos_x, pos_y,
        static_cast<int>(width),
        static_cast<int>(height),
        0
    );
}

//==============================================================================
Win32Window::Win32Window(const vk::Instance &instance,
                         const uint32_t width, const uint32_t height) :
    _hinstance       { nullptr },
    _hwindow         { nullptr },
    _surface         { nullptr },
    _width           { width  },
    _height          { height },
    _screen_width    { static_cast<uint32_t>(::GetSystemMetrics(SM_CXSCREEN)) },
    _screen_height   { static_cast<uint32_t>(::GetSystemMetrics(SM_CYSCREEN)) },
    _fullscreen      { false },
    _running         { false },
    _instance        { instance }
{
    if(_width == 0 || _height == 0) {
        _width  = static_cast<uint32_t>(_screen_width  * 0.75f);
        _height = static_cast<uint32_t>(_screen_height * 0.75f);
    }
}

Win32Window::~Win32Window() {
    CONSOLE_INFO("");

    ::vkDestroySurfaceKHR(_instance, _surface, nullptr);
}

#endif // WIN32