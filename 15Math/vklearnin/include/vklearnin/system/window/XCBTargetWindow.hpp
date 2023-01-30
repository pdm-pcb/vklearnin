#ifdef VKL_LINUX

#ifndef VKLEARNIN_SYSTEM_WINDOW_XCBTARGETWINDOW_HPP
#define VKLEARNIN_SYSTEM_WINDOW_XCBTARGETWINDOW_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class XCBTargetWindow {
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

    XCBTargetWindow() = delete;

private:
    // status variable
    static bool _carry_on;

    // Helps with centering the window on screen
    static struct CenterPos {
        int32_t x = 0;
        int32_t y = 0;
    } _center;

    static vk::SurfaceKHR _surface;

    struct MotifHints {
        uint32_t flags       = 0u;
        uint32_t functions   = 0u;
        uint32_t decorations = 0u;
        int32_t  input_mode  = 0;
        uint32_t status      = 0u;
    };

    static ::xcb_connection_t  *_connection;
    static ::xcb_screen_t      *_screen;
    static ::xcb_key_symbols_t *_key_symbols;
    static ::xcb_window_t       _window;
    static ::xcb_atom_t         _delete_atom;
    static ::xcb_atom_t         _wm_state_atom;

    static void _query_randr();
    static void _redirect_delete();
    static void _remove_decorations();
    static void _acquire_multiuse_atoms();

    static void _init();

    // Make it just right
    static void _size_and_place();
};

} // namespace vkl

#endif // VKLEARNIN_SYSTEM_WINDOW_XCBTARGETWINDOW_HPP

#endif // VKL_LINUX