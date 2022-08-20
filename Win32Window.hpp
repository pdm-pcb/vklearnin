#ifndef VKL_WIN32WINDOW_HPP
#define VKL_WIN32WINDOW_HPP

#include <vulkan/vulkan.h>
#include <Windows.h>

#include <cstdint>
#include <utility>

class RenderLoop;

class Win32Window {
public:
    bool message_loop(RenderLoop &render_loop);

    void init_window();
    void init_surface();

    inline const ::VkSurfaceKHR & surface() const { return _surface; }
    inline const ::VkOffset2D   & offset()  const { return _offset;  }
    inline const ::VkExtent2D   & extent()  const { return _extent;  }

    Win32Window(const uint32_t x_res, const uint32_t y_res,
                const int32_t x_offset, const int32_t y_offset,
                const ::VkInstance &instance);
    ~Win32Window();

    Win32Window() = delete;

private:
    ::HINSTANCE _hinstance;
    ::HWND      _hwindow;

    ::VkSurfaceKHR _surface;
    ::VkOffset2D   _offset;
    ::VkExtent2D   _extent;

    bool _running;
    bool _resized;
    
    uint32_t _display_x;
    uint32_t _display_y;
    uint32_t _launch_x;
    uint32_t _launch_y;

    const ::VkInstance &_instance;

    static ::LRESULT CALLBACK
    _message_dispatch(::HWND window, ::UINT msg, ::WPARAM wparam,
                      ::LPARAM lparam);
    ::LRESULT
    _message_handler(::HWND window, ::UINT msg, ::WPARAM wparam,
                     ::LPARAM lparam);

    void _build_window(const uint32_t x_res, const uint32_t y_res);
};

#endif // VKL_WIN32WINDOW_HPP