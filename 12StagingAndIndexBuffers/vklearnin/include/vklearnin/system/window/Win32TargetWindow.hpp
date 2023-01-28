#ifdef VKL_WINDOWS

#ifndef VKLEARNIN_SYSTEM_WINDOW_WIN32TARGETWINDOW_HPP
#define VKLEARNIN_SYSTEM_WINDOW_WIN32TARGETWINDOW_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Win32TargetWindow final {
public:
    // Give the OS a moment to speak up
    static bool message_loop();

    // Spawn (and size and place?) a native window
    static void spawn_window(const uint32_t width  = 0u,
                             const uint32_t height = 0u,
                             const int32_t  pos_x  = 0u,
                             const int32_t  pos_y  = 0u);

    // Manage the Vulkan surface 
    static void create_surface();
    static void destroy_surface();

    static inline const auto & surface() { return _surface; }

    // Only one target window at a time
    Win32TargetWindow() = delete;

private:
    // status variable
    static bool _carry_on;

    // Win32 specifics
    static ::HWND   _window;
    static ::LPCSTR _classname;
    static ::LPCSTR _window_title;
    
    // Helps with centering the window on screen
    static struct CenterPos {
        int32_t x = 0;
        int32_t y = 0;
    } _center;

    // Vulkan specifics
    static vk::SurfaceKHR _surface;

    // The way in for Windows
    static ::LRESULT CALLBACK
    _wndproc(::HWND window, ::UINT message, ::WPARAM wparam, ::LPARAM lparam);

    static void _init();

    // Make it just right
    static void _size_and_place();
};

} // namespace vkl

#endif // VKLEARNIN_SYSTEM_WIN32TARGETWINDOW_HPP

#endif // VKL_WINDOWS