#ifdef __linux__
#ifndef VKL_X11WINDOW_HPP
#define VKL_X11WINDOW_HPP

#include <xcb/xcb.h>
#include <vulkan/vulkan.h>

#include <cstdint>

class X11Window {
    struct MotifHints {
        uint32_t   flags;
        uint32_t   functions;
        uint32_t   decorations;
        int32_t    input_mode;
        uint32_t   status;
    };

public:
    bool message_loop();

    void init_window();
    void init_surface();

    inline ::VkSurfaceKHR & surface() { return _surface; }
    inline const ::VkOffset2D & offset() const { return _offset; }
    inline const ::VkExtent2D & extent() const { return _extent; }
    inline uint32_t width()  const { return _width;  }
    inline uint32_t height() const { return _height; }

    X11Window(const uint32_t width, const uint32_t height,
              const int32_t x_offset, const int32_t y_offset,
              const ::VkInstance &instance);
    ~X11Window();

    X11Window() = delete;

private:
    ::xcb_connection_t  *_connection;
    ::xcb_screen_t      *_screen;
    ::xcb_key_symbols_t *_key_symbols;
    ::xcb_window_t       _window;
    ::xcb_atom_t         _delete_atom;
    ::xcb_atom_t         _wm_state_atom;
    ::xcb_atom_t         _fullscreen_atom;

    ::xcb_client_message_event_t _fullscreen_event;

    ::VkSurfaceKHR _surface;
    ::VkOffset2D   _offset;
    ::VkExtent2D   _extent;
    
    uint32_t _width;
    uint32_t _height;
    uint32_t _x_pos;
    uint32_t _y_pos;

    uint32_t _screen_width;
    uint32_t _screen_height;
    int32_t  _screen_x_offset;
    int32_t  _screen_y_offset;

    bool _fullscreen;
    bool _running;

    const ::VkInstance &_instance;

    void _query_randr();
    void _redirect_delete();
    void _remove_decorations();
    void _acquire_multiuse_atoms();
    void _size_window(const uint32_t width, const uint32_t height);
    void _center_window();
};

#endif // VKL_X11WINDOW_HPP
#endif // __liunx__