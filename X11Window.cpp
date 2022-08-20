#include "common.hpp"
#include "X11Window.hpp"

#include "Instance.hpp"
#include "RenderLoop.hpp"

bool X11Window::message_loop(RenderLoop &render_loop) {
    ::xcb_generic_event_t *gen_event = nullptr;

    while((gen_event = ::xcb_poll_for_event(_connection))) {
        switch(gen_event->response_type & ~0x80) {
            case XCB_CLIENT_MESSAGE: {
                auto *event =
                    reinterpret_cast<::xcb_client_message_event_t *>(gen_event);
                if(event->data.data32[0] == _wm_delete) {
                    CONSOLE_WARN("Well, looks like we made it.");
                }
                break;
            }

            case XCB_CONFIGURE_NOTIFY: {
                auto *event =
                    reinterpret_cast<::xcb_configure_notify_event_t *>(gen_event);

                CONSOLE_WARN(
                    "offset: {}x{} size: {}x{}",
                    event->x, event->y,
                    event->width, event->height
                );
            }
            
            case XCB_KEY_PRESS: {
                auto *event =
                    reinterpret_cast<::xcb_key_press_event_t *>(gen_event);
                switch(event->detail) {
                    case 9:
                        ::xcb_destroy_window(_connection, _window);
                        _running = false;
                        break;
                }
                break;
            }
            case XCB_KEY_RELEASE:
                break;
        }

        free(gen_event);
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

        ::XCB_EVENT_MASK_KEY_PRESS |
        ::XCB_EVENT_MASK_KEY_RELEASE
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

void X11Window::_build_window(const uint32_t x_res, const uint32_t y_res) {
    _query_randr();

    _extent = { x_res, y_res };

    uint32_t pos_x = _display_x / 2 - (_extent.width  / 2);
    uint32_t pos_y = _display_y / 2 - (_extent.height / 2);
    
    uint16_t value_mask = ::XCB_CONFIG_WINDOW_X | ::XCB_CONFIG_WINDOW_Y;
    const uint32_t value_list[] { pos_x, pos_y };

    ::xcb_configure_window(
        _connection,
        _window,
        value_mask,
        value_list
    );

    ::xcb_map_window(_connection, _window);
    ::xcb_flush(_connection);
}

void X11Window::_query_randr() {
    auto *reply = xcb_randr_get_screen_resources_current_reply(
        _connection,
        xcb_randr_get_screen_resources_current(_connection, _window),
        nullptr
    );

    auto ts = reply->config_timestamp;
    int  count = xcb_randr_get_screen_resources_current_outputs_length(reply);
    auto *outputs = xcb_randr_get_screen_resources_current_outputs(reply);



    for(int dpy = 0; dpy < count; dpy++) {
        auto *output = xcb_randr_get_output_info_reply(
            _connection,
            xcb_randr_get_output_info(
                _connection,
                outputs[dpy],
                ts
            ),
            nullptr
        );
        if(output == nullptr) {
            continue;
            CONSOLE_WARN("Xrandr output {} provided nullptr", dpy);
        }

        if(output->crtc == XCB_NONE ||
           output->connection == XCB_RANDR_CONNECTION_DISCONNECTED)
        {
            continue;
            CONSOLE_WARN("Xrandr output {} is invalid or disconnected", dpy);
        }

        auto *crtc = xcb_randr_get_crtc_info_reply(
            _connection,
            xcb_randr_get_crtc_info(_connection, output->crtc, ts),
            nullptr
        );

        if(crtc->width > _display_x && crtc->height > _display_y) {
            _display_x = crtc->width;
            _display_y = crtc->height;

            _offset.x = crtc->x;
            _offset.y = crtc->y;
        }

        free(crtc);
        free(output);
    }

    free(reply);

    CONSOLE_TRACE(
        "Display with resolution {}x{} selected.",
        _display_x, _display_y
    );
}

X11Window::X11Window(const uint32_t x_res, const uint32_t y_res,
                     const int32_t x_offset, const int32_t y_offset,
                     const ::VkInstance &instance) :
    _connection { nullptr },
    _window     { 0u },
    _screen     { nullptr },
    _wm_delete  { 0u },
    _wm_proto   { 0u },
    _surface    { 0u },
    _offset     { x_offset, y_offset },
    _extent     { x_res, y_res },
    _running    { false },
    _resized    { false },
    _display_x  { 0u },
    _display_y  { 0u },
    _launch_x   { x_res },
    _launch_y   { y_res },
    _instance   { instance }
{
    CONSOLE_INFO("");
}

X11Window::~X11Window() {
    CONSOLE_INFO("");
    ::vkDestroySurfaceKHR(_instance, _surface, nullptr);

    ::xcb_destroy_window(_connection, _window);
    ::xcb_disconnect(_connection);
}