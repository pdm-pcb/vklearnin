#ifndef VKL_X11WINDOW_HPP
#define VKL_X11WINDOW_HPP

#include <cstdint>
#include <xcb/xcb.h>
#include <vulkan/vulkan.h>

class Instance;

class X11Window {
public:
    void render_loop();

    void init_window();
    void init_surface();

    const ::VkSurfaceKHR & surface() const { return _surface; }
    uint32_t x_res() const { return _x_res; }
    uint32_t y_res() const { return _y_res; }

    X11Window(const uint32_t x_res, const uint32_t y_res,
              const Instance &instance);
    ~X11Window();

    X11Window() = delete;

private:
    const Instance &_instance;

    uint32_t _x_res;
    uint32_t _y_res;

    ::xcb_connection_t *_connection;
    ::xcb_window_t      _window;
    ::xcb_screen_t     *_screen;
    ::xcb_atom_t        _wm_delete;
    ::xcb_atom_t        _wm_proto;

    ::VkSurfaceKHR _surface;
};

#endif // VKL_X11WINDOW_HPP