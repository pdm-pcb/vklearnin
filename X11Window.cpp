#include "common.hpp"
#include "X11Window.hpp"

#include "Instance.hpp"
#include "RenderLoop.hpp"

bool X11Window::message_loop(RenderLoop &render_loop) {
    ::xcb_generic_event_t *event = nullptr;

    while((event = ::xcb_poll_for_event(_connection))) {
        switch(event->response_type & 0x7f) {
            case XCB_CLIENT_MESSAGE: {
                auto *msg =
                    reinterpret_cast<::xcb_client_message_event_t *>(event);
                if(msg->data.data32[0] == _wm_delete) {
                    CONSOLE_WARN("Well, looks like we made it.");
                    _running = false;
                }
                break;
            }

            case XCB_DESTROY_NOTIFY:
                _running = false;
                break;

            case XCB_CONFIGURE_NOTIFY: {
                auto *config =
                    reinterpret_cast<::xcb_configure_notify_event_t *>(event);

                CONSOLE_WARN(
                    "offset: {}x{} size: {}x{}",
                    config->x, config->y,
                    config->width, config->height
                );
            }
            
            case XCB_KEY_PRESS: {
                auto *press =
                    reinterpret_cast<::xcb_key_press_event_t *>(event);
                switch(press->detail) {
                    case 0x9: // Escape
                        _running = false;
                        break;
                }
                break;
            }

            case XCB_KEY_RELEASE: {
                auto *release =
                    reinterpret_cast<::xcb_key_press_event_t *>(event);
                switch(release->detail) {
                    case 0x26: // 'A'
                        if(_extent.width == 1024)
                            _build_window(1280, 1024);
                        else
                            _build_window(1024, 768);
                        break;
                }
                break;
            }
        }

        free(event);
    }
    
    render_loop.resized(_resized);
    return _running;
}

void X11Window::init_window() {
    CONSOLE_INFO("");

    int screenp = 0;
    _connection = ::xcb_connect(nullptr, &screenp);

    if(::xcb_connection_has_error(_connection)) {
        CONSOLE_CRITICAL("Could not connect to X server.");
    }

    ::xcb_screen_iterator_t screen_iter =
        ::xcb_setup_roots_iterator(::xcb_get_setup(_connection));

    for(int screen = screenp; screen > 0; --screen) {
        ::xcb_screen_next(&screen_iter);
    }

    _screen = screen_iter.data;

    _window = ::xcb_generate_id(_connection);

    uint32_t vakue_mask = ::XCB_CW_BACK_PIXEL | ::XCB_CW_EVENT_MASK;
    uint32_t value_list[] {
        _screen->black_pixel,
        XCB_CONFIGURE_NOTIFY |
        ::XCB_EVENT_MASK_EXPOSURE |
        ::XCB_EVENT_MASK_KEY_PRESS |
        ::XCB_EVENT_MASK_KEY_RELEASE |
        ::XCB_EVENT_MASK_STRUCTURE_NOTIFY |
        ::XCB_EVENT_MASK_RESIZE_REDIRECT,

    };

    ::xcb_create_window(
        _connection,
        XCB_COPY_FROM_PARENT,
        _window,
        _screen->root,
        0, 0,
        static_cast<uint16_t>(_extent.width),
        static_cast<uint16_t>(_extent.height),
        0,
        ::XCB_WINDOW_CLASS_INPUT_OUTPUT,
        _screen->root_visual,
        vakue_mask,
        value_list
    );

    ::xcb_intern_atom_cookie_t wm_delete_cookie =
        ::xcb_intern_atom(
            _connection,
            0,
            strlen("WM_DELETE_WINDOW"),
            "WM_DELETE_WINDOW"
        );

    ::xcb_intern_atom_cookie_t wm_protocols_cookie =
        ::xcb_intern_atom(
            _connection,
            0,
            strlen("WM_PROTOCOLS"),
            "WM_PROTOCOLS"
        );

    ::xcb_intern_atom_reply_t *delete_reply =
        ::xcb_intern_atom_reply(
            _connection,
            wm_delete_cookie,
            nullptr
        );

    ::xcb_intern_atom_reply_t *protocols_reply =
        ::xcb_intern_atom_reply(
            _connection,
            wm_protocols_cookie,
            nullptr
        );

    _wm_delete = delete_reply->atom;
    _wm_proto  = protocols_reply->atom;

    ::xcb_map_window(_connection, _window);
    ::xcb_flush(_connection);

    _build_window(_extent.width, _extent.height);
    _running = true;
}

void X11Window::init_surface() {
    ::VkXcbSurfaceCreateInfoKHR surface_info { };
    surface_info.sType = ::VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    surface_info.connection = _connection;
    surface_info.window = _window;

    ::VkResult result = vkCreateXcbSurfaceKHR(
        _instance,
        &surface_info,
        nullptr,
        &_surface
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_CRITICAL("Unable to create XCB surface.");
    }
}

void X11Window::_build_window(const uint32_t width, const uint32_t height) {
    _query_randr();

    _extent = { width, height };

    uint32_t pos_x = (_display_xres / 2 - (_extent.width  / 2));
    uint32_t pos_y = (_display_yres / 2 - (_extent.height / 2));
    
    uint32_t value_mask =
        ::XCB_CONFIG_WINDOW_X |
        ::XCB_CONFIG_WINDOW_Y |
        ::XCB_CONFIG_WINDOW_WIDTH |
        ::XCB_CONFIG_WINDOW_HEIGHT;

    int32_t value_list[] {
        static_cast<int32_t>(pos_x) + _display_xoff,
        static_cast<int32_t>(pos_y) + _display_yoff,
        static_cast<int32_t>(_extent.width),
        static_cast<int32_t>(_extent.height)
    };

    ::xcb_configure_window(
        _connection,
        _window,
        value_mask,
        value_list
    );
}

void X11Window::_query_randr() {
    auto *reply = xcb_randr_get_screen_resources_current_reply(
        _connection,
        xcb_randr_get_screen_resources_current(_connection, _window),
        nullptr
    );

    auto reply_ts = reply->config_timestamp;

    int count = xcb_randr_get_screen_resources_current_outputs_length(reply);

    if(count == 0) {
        CONSOLE_CRITICAL("xrandr could not provide any outputs");
    }

    auto *outputs = xcb_randr_get_screen_resources_current_outputs(reply);
    auto *output = xcb_randr_get_output_info_reply(
        _connection,
        xcb_randr_get_output_info(
            _connection,
            outputs[0],
            reply_ts
        ),
        nullptr
    );

    if(output == nullptr) {
        CONSOLE_CRITICAL("Primary xrandr output provided nullptr");
    }

    if(output->crtc == XCB_NONE ||
        output->connection == XCB_RANDR_CONNECTION_DISCONNECTED)
    {
        CONSOLE_CRITICAL("Primary xrandr output is invalid or disconnected");
    }

    auto *crtc = xcb_randr_get_crtc_info_reply(
        _connection,
        xcb_randr_get_crtc_info(_connection, output->crtc, reply_ts),
        nullptr
    );

    _display_xres = crtc->width;
    _display_yres = crtc->height;

    _display_xoff = crtc->x;
    _display_yoff = crtc->y;

    free(crtc);
    free(output);
    free(reply);

    CONSOLE_TRACE(
        "Display 0 with resolution {}x{}, offset {}x{} selected.",
        _display_xres, _display_yres,
        _display_xoff, _display_yoff
    );
}

X11Window::X11Window(const uint32_t width, const uint32_t height,
                     const int32_t x_offset, const int32_t y_offset,
                     const ::VkInstance &instance) :
    _connection    { nullptr },
    _window        { 0u },
    _screen        { nullptr },
    _wm_delete     { 0u },
    _wm_proto      { 0u },
    _surface       { 0u },
    _offset        { x_offset, y_offset },
    _extent        { width, height },
    _launch_width  { width  },
    _launch_height { height },
    _display_xres  { 0u },
    _display_yres  { 0u },
    _display_xoff  { 0 },
    _display_yoff  { 0 },
    _running       { false },
    _resized       { false },
    _instance      { instance }
{
    CONSOLE_INFO("");
}

X11Window::~X11Window() {
    CONSOLE_INFO("");
    ::vkDestroySurfaceKHR(_instance, _surface, nullptr);

    ::xcb_destroy_window(_connection, _window);
    ::xcb_disconnect(_connection);
}