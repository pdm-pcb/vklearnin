#ifdef _WIN32

#ifndef VKLEARNIN_SYSTEM_WIN32TARGETWINDOW_HPP
#define VKLEARNIN_SYSTEM_WIN32TARGETWINDOW_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Win32TargetWindow final {
public:
    // Give the OS a moment to speak up
    static void message_loop();

    // Setting up and shutting down the native window
    static void spawn_window(const uint16_t width = 0u,
                             const uint16_t height = 0u);
    static void create_surface();
    static void destroy_surface();
    static void shutdown();

    // For those who would like to know
    inline static vk::SurfaceKHR & surface() { return _surface; }

    // Standard fare
    Win32TargetWindow() = delete;
    ~Win32TargetWindow() = delete;

    Win32TargetWindow(Win32TargetWindow &&) = delete;
    Win32TargetWindow(const Win32TargetWindow &) = delete;

    Win32TargetWindow & operator=(Win32TargetWindow &&) = delete;
    Win32TargetWindow & operator=(const Win32TargetWindow &) = delete;

private:
    // Win32 specifics
    static ::HWND   _window;
    static ::LPCSTR _classname;
    static ::LPCSTR _window_title;
    static ::HDC    _device;
    static ::LPBYTE _raw_message;
    
    // Helps with centering the window on screen
    static struct CenterPos {
        uint16_t x;
        uint16_t y;
    } _center;

    // Vulkan specifics
    static vk::SurfaceKHR _surface;

    // The way in for Windows
    static ::LRESULT CALLBACK
    _wndproc(::HWND window, ::UINT msg, ::WPARAM wparam, ::LPARAM lparam);

    static void _register_input();

    // Make it just right
    static void _size_and_place(const uint16_t width, const uint16_t height,
                                const uint16_t pos_x, const uint16_t pos_y);
};

} // namespace vkl

#endif // VKLEARNIN_SYSTEM_WIN32TARGETWINDOW_HPP

#endif // _WIN32