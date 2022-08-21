#ifdef __linux__
#ifndef VKL_X11WINDOW_HPP
#define VKL_X11WINDOW_HPP

#include <cstdint>
#include <xcb/xcb.h>
#include <vulkan/vulkan.h>

class RenderLoop;

class X11Window {
    struct MotifHints {
        uint32_t   flags;
        uint32_t   functions;
        uint32_t   decorations;
        int32_t    input_mode;
        uint32_t   status;
    };

public:
    bool message_loop(RenderLoop &render_loop);

    void init_window();
    void init_surface();

    inline const ::VkSurfaceKHR & surface() const { return _surface; }
    inline const ::VkOffset2D   & offset()  const { return _offset;  }
    inline const ::VkExtent2D   & extent()  const { return _extent;  }

    X11Window(const uint32_t width, const uint32_t height,
              const int32_t x_offset, const int32_t y_offset,
              const ::VkInstance &instance);
    ~X11Window();

    X11Window() = delete;

private:
    ::xcb_connection_t  *_connection;
    ::xcb_window_t       _window;
    ::xcb_screen_t      *_screen;
    ::xcb_atom_t         _wm_delete;
    ::xcb_atom_t         _wm_proto;
    ::xcb_key_symbols_t *_key_symbols;

    ::VkSurfaceKHR _surface;
    ::VkOffset2D   _offset;
    ::VkExtent2D   _extent;
    
    uint32_t _width;
    uint32_t _height;

    uint32_t _display_xres;
    uint32_t _display_yres;
    int32_t  _display_xoff;
    int32_t  _display_yoff;

    bool _running;
    bool _resized;
    bool _fullscreen;

    const ::VkInstance &_instance;

    void _query_randr();
    void _size_window(const uint32_t width, const uint32_t height);
};

#endif // VKL_X11WINDOW_HPP
#endif // __liunx__