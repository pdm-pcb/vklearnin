#ifndef VKLEARNIN_SYSTEM_WINDOW_WIN32TARGETWINDOW_HPP
#define VKLEARNIN_SYSTEM_WINDOW_WIN32TARGETWINDOW_HPP

#ifdef VKL_WINDOWS

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Win32TargetWindow final {
public:
    // Give the OS a moment to speak up
    static void message_loop();

    static void init();
    static void shutdown();

    // Spawn (and size and place?) a native window
    static void spawn_window(uint32_t const width  = 0u,
                             uint32_t const height = 0u,
                             int32_t  const pos_x  = 0u,
                             int32_t  const pos_y  = 0u);

    // Manage the Vulkan surface
    static void create_surface();
    static void destroy_surface();

    inline static auto const& surface() { return _surface; }

    // Only one target window at a time
    Win32TargetWindow() = delete;

private:
    // Helps with centering the window on screen
    static struct ScreenPos {
        int32_t x = 0;
        int32_t y = 0;
    } _center;

    // Vulkan specifics
    static vk::SurfaceKHR _surface;

    // Win32 specifics
    static ::HWND   _window;
    static ::LPCSTR _classname;
    static ::LPCSTR _window_title;
    static ::HDC    _device;
    static ::LPBYTE _raw_message;

    // The way in for Windows
    static ::LRESULT CALLBACK
    _wndproc(::HWND window, ::UINT message, ::WPARAM wparam, ::LPARAM lparam);

    static void _register_input();
    static void _restrict_cursor();
    static void _release_cursor();

    // Make it just right
    static void _size_and_place();
};

} // namespace vkl

#endif // VKL_WINDOWS

#endif // VKLEARNIN_SYSTEM_WINDOW_WIN32TARGETWINDOW_HPP