#ifdef WIN32
#ifndef VKLEARNIN_WIN32WINDOW_HPP
#define VKLEARNIN_WIN32WINDOW_HPP

#include <vulkan/vulkan.h>
#include <Windows.h>

#include <cstdint>
#include <utility>

class RenderLoop;

class Win32Window {
public:
    bool message_loop();

    void init_window();
    void init_surface();

    inline ::VkSurfaceKHR & surface() { return _surface; }
    inline uint32_t width()  const    { return _width;   }
    inline uint32_t height() const    { return _height;  }

    Win32Window(const ::VkInstance &instance,
                const uint32_t width = 0, const uint32_t height = 0);
    ~Win32Window();

    Win32Window() = delete;

private:
    ::HINSTANCE _hinstance;
    ::HWND      _hwindow;

    ::VkSurfaceKHR _surface;

    uint32_t _width;
    uint32_t _height;
    uint32_t _screen_width;
    uint32_t _screen_height;

    bool _fullscreen;
    bool _running;

    const ::VkInstance &_instance;

    static ::LRESULT CALLBACK
    _message_dispatch(::HWND window, ::UINT msg, ::WPARAM wparam,
                      ::LPARAM lparam);
    ::LRESULT
    _message_handler(::HWND window, ::UINT msg, ::WPARAM wparam,
                     ::LPARAM lparam);

    void _size_and_center(const uint32_t width, const uint32_t height);
};

#endif // VKLEARNIN_WIN32WINDOW_HPP
#endif // WIN32