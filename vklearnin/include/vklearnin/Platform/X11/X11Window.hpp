#ifdef __linux__
#ifndef VKLEARNIN_X11WINDOW_HPP
#define VKLEARNIN_X11WINDOW_HPP

#include "vklearnin/pch.hpp"

class RenderLoop;

class X11Window {
public:
    static bool up;
    static bool down;
    static bool left;
    static bool right;
    static bool w;
    static bool a;
    static bool s;
    static bool d;
    static bool ctrl;
    static bool space;

    bool message_loop();

    void init_window();
    void init_surface();

    inline vk::SurfaceKHR & surface() { return _surface; }
    inline uint32_t width()  const    { return _width;   }
    inline uint32_t height() const    { return _height;  }

    X11Window(const vk::Instance &instance,
              const uint32_t width = 0, const uint32_t height = 0);
    ~X11Window();

    X11Window() = delete;

private:
    struct MotifHints {
        uint32_t flags;
        uint32_t functions;
        uint32_t decorations;
        int32_t  input_mode;
        uint32_t status;
    };

    ::xcb_connection_t  *_connection;
    ::xcb_screen_t      *_screen;
    ::xcb_key_symbols_t *_key_symbols;
    ::xcb_window_t       _window;
    ::xcb_atom_t         _delete_atom;
    ::xcb_atom_t         _wm_state_atom;
    ::xcb_atom_t         _fullscreen_atom;

    ::xcb_client_message_event_t _fullscreen_event;

    vk::SurfaceKHR _surface;
    
    uint32_t _width;
    uint32_t _height;

    uint32_t _screen_width;
    uint32_t _screen_height;
    int32_t  _screen_x_offset;
    int32_t  _screen_y_offset;

    bool _fullscreen;
    bool _running;

    const vk::Instance &_instance;

    void _query_randr();
    void _redirect_delete();
    void _remove_decorations();
    void _acquire_multiuse_atoms();
    void _size_window(const uint32_t width, const uint32_t height);
    void _center_window();
};

#endif // VKLEARNIN_X11WINDOW_HPP
#endif // __liunx__