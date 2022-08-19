#ifndef VKL_WIN32WINDOW_HPP
#define VKL_WIN32WINDOW_HPP

#include <vulkan/vulkan.h>
#include <cstdint>
#include <Windows.h>

class Win32Window {
public:
    bool message_loop();

    void init_window();
    void init_surface();

    const ::VkSurfaceKHR & surface() const { return _surface; }
    uint32_t x_res() const { return _x_res; }
    uint32_t y_res() const { return _y_res; }

    Win32Window(const uint32_t x_res, const uint32_t y_res,
                const ::VkInstance &instance);
    ~Win32Window();

    Win32Window() = delete;

private:
    ::HINSTANCE _hinstance;
    ::HWND      _hwindow;

    ::VkSurfaceKHR _surface;
    
    bool _running;

    uint32_t _x_res;
    uint32_t _y_res;

    const ::VkInstance &_instance;

    static ::LRESULT CALLBACK
    _message_dispatch(::HWND window, ::UINT msg, ::WPARAM wparam,
                      ::LPARAM lparam);
    ::LRESULT
    _message_handler(::HWND window, ::UINT msg, ::WPARAM wparam,
                     ::LPARAM lparam);
};

#endif // VKL_WIN32WINDOW_HPP