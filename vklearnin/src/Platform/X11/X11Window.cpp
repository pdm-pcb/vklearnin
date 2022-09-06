#ifdef __linux__

#include "vklearnin/common.hpp"
#include "vklearnin/Platform/X11/X11Window.hpp"

#include "vklearnin/Instance.hpp"
#include "vklearnin/RenderLoop.hpp"

bool X11Window::up    = false;
bool X11Window::down  = false;
bool X11Window::left  = false;
bool X11Window::right = false;
bool X11Window::w     = false;
bool X11Window::a     = false;
bool X11Window::s     = false;
bool X11Window::d     = false;
bool X11Window::ctrl  = false;
bool X11Window::space = false;

using client_msg        = ::xcb_client_message_event_t *;
using config_notify     = ::xcb_configure_notify_event_t *;
using keypress_notify   = ::xcb_key_press_event_t *;
using keyrelease_notify = ::xcb_key_release_event_t *;
using property_notify   = ::xcb_property_notify_event_t *;

static constexpr uint32_t XCB_EVENT_RESPONSE_TYPE_MASK = ~0x80u;

//==============================================================================
bool X11Window::message_loop() {
    ::xcb_generic_event_t *event = nullptr;

    while((event = ::xcb_poll_for_event(_connection))) {       
        uint32_t event_type =
            event->response_type & XCB_EVENT_RESPONSE_TYPE_MASK;
        switch(event_type) {
            case 0:
                break;

            case XCB_KEY_PRESS: {
                CONSOLE_TRACE("{}", "XCB_KEY_PRESS");

                auto *press = reinterpret_cast<keypress_notify>(event);
                auto key = ::xcb_key_symbols_get_keysym(
                    _key_symbols,
                    press->detail,
                    0
                );

                switch(key) {
                    case XK_Escape:
                        _running = false;
                        break;
                }
                break;
            }

            case XCB_KEY_RELEASE: {
                CONSOLE_TRACE("{}", "XCB_KEY_RELEASE");

                auto *release = reinterpret_cast<keyrelease_notify>(event);
                auto key = ::xcb_key_symbols_get_keysym(
                    _key_symbols,
                    release->detail,
                    0
                );

                switch(key) {
                    case XK_Return: {
                        if((release->state & ::XCB_MOD_MASK_1) ||
                           (release->state & ::XCB_MOD_MASK_5))
                        {
                            ::xcb_send_event(
                                _connection,
                                0u,
                                _screen->root,
                                ::XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
                                ::XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY,
                                reinterpret_cast<const char*>(&_fullscreen_event)
                            );
                            
                            ::xcb_flush(_connection);
                        }
                        break;
                    }
                }
                break;
            }

            case XCB_BUTTON_PRESS: {
                CONSOLE_TRACE("{}", "XCB_BUTTON_PRESS");

                break;
            }

            case XCB_BUTTON_RELEASE: {
                CONSOLE_TRACE("{}", "XCB_BUTTON_RELEASE");

                break;
            }

            case XCB_EXPOSE:
                CONSOLE_TRACE("{}", "XCB_EXPOSE");
                
                break;

            case XCB_GRAPHICS_EXPOSURE:
                CONSOLE_TRACE("{}", "XCB_BUTTON_RELEASE");

                break;

            case XCB_MAP_NOTIFY:
                CONSOLE_TRACE("{}", "XCB_MAP_NOTIFY");
                break;

            case XCB_CONFIGURE_NOTIFY:  {
                CONSOLE_TRACE("{}", "XCB_CONFIGURE_NOTIFY");

                auto *config = reinterpret_cast<config_notify>(event);
                if(config->width != _width || config->height != _height) {
                    _width         = config->width;
                    _height        = config->height;
                }
                break;
            }

            case XCB_CLIENT_MESSAGE: {
                CONSOLE_TRACE("{}", "XCB_CLIENT_MESSAGE");

                auto *msg = reinterpret_cast<client_msg>(event);
                if(msg->data.data32[0] == _delete_atom) {
                    _running = false;
                }
                break;
            }

            case XCB_GE_GENERIC:
                break;
            
            default:
                CONSOLE_WARN("Unknown XCB message? '{}'", event_type);
                break;
        }

        free(event);
    }

    return _running;
}

//==============================================================================
void X11Window::init_window() {
    CONSOLE_INFO("");

    uint32_t vakue_mask = ::XCB_CW_BACK_PIXEL |
                          ::XCB_CW_EVENT_MASK;
    uint32_t value_list[] {
        _screen->black_pixel,
        ::XCB_EVENT_MASK_KEY_PRESS |
        ::XCB_EVENT_MASK_KEY_RELEASE |
        ::XCB_EVENT_MASK_BUTTON_PRESS |
        ::XCB_EVENT_MASK_BUTTON_RELEASE |
        ::XCB_EVENT_MASK_EXPOSURE |
        ::XCB_EVENT_MASK_STRUCTURE_NOTIFY
    };

    ::xcb_create_window(
        _connection,
        XCB_COPY_FROM_PARENT,
        _window,
        _screen->root,
        0, 0,
        static_cast<uint16_t>(_width),
        static_cast<uint16_t>(_height),
        0,
        ::XCB_WINDOW_CLASS_INPUT_OUTPUT,
        _screen->root_visual,
        vakue_mask,
        value_list
    );

    _redirect_delete();
    _remove_decorations();
    _acquire_multiuse_atoms();

    ::xcb_map_window(_connection, _window);
    ::xcb_set_input_focus(
        _connection,
        ::XCB_INPUT_FOCUS_POINTER_ROOT,
        _window,
        XCB_CURRENT_TIME
    );

    _center_window();
    _running = true;
}

//==============================================================================
void X11Window::init_surface() {
    CONSOLE_INFO("");

    _instance.destroy(_surface);

    vk::XcbSurfaceCreateInfoKHR surface_info {
        .connection = _connection,
        .window = _window,
    };

    _surface = _instance.createXcbSurfaceKHR(surface_info);
}

//==============================================================================
void X11Window::_query_randr() {
    CONSOLE_INFO("");

    auto reply = ::xcb_randr_get_monitors_reply(
        _connection,
        ::xcb_randr_get_monitors(
            _connection,
            _screen->root,
            0u
        ),
        nullptr
    );

    auto iter = ::xcb_randr_get_monitors_monitors_iterator(reply);
    CONSOLE_TRACE("Found {} monitors", iter.rem);

    while (iter.rem > 0) {
        ::xcb_randr_monitor_info_t *info = iter.data;
        CONSOLE_TRACE(
            "\n\tMonitor Name: {}"
            "\n\tResolution  : {}x{} +{}+{}"
            "\n\tPrimary     : {}",
            info->name,
            info->width, info->height,
            info->x, info->y,
            (info->primary ? "Yes" : "No")
        );

        if(info->primary) {
            _screen_width    = info->width;
            _screen_height   = info->height;
            _screen_x_offset = info->x;
            _screen_y_offset = info->y;
        }

        ::xcb_randr_monitor_info_next(&iter);
    }

    free(reply);
}

//==============================================================================
void X11Window::_redirect_delete() {
    CONSOLE_INFO("");

    auto *delete_reply = ::xcb_intern_atom_reply(
        _connection,
        ::xcb_intern_atom(
            _connection,
            0,
            sizeof("WM_DELETE_WINDOW") - 1,
            "WM_DELETE_WINDOW"
        ),
        nullptr
    );

    _delete_atom = delete_reply->atom;

    auto *protocols_reply = ::xcb_intern_atom_reply(
        _connection,
        ::xcb_intern_atom(
            _connection,
            0,
            sizeof("WM_PROTOCOLS") - 1,
            "WM_PROTOCOLS"
        ),
        nullptr
    );

    ::xcb_change_property(
        _connection,
        ::XCB_PROP_MODE_REPLACE,
        _window,
        protocols_reply->atom,
        ::XCB_ATOM_ATOM,
        32u,
        1u,
        &_delete_atom
    );

    free(delete_reply);
    free(protocols_reply);
}

//==============================================================================
void X11Window::_remove_decorations() {
    CONSOLE_INFO("");

    auto *motif_reply = ::xcb_intern_atom_reply(
        _connection,
        ::xcb_intern_atom(
            _connection,
            0,
            sizeof("_MOTIF_WM_HINTS") - 1,
            "_MOTIF_WM_HINTS"
        ),
        nullptr
    );

    MotifHints hints {
        .flags = 2u,
        .functions = 0u,
        .decorations = 0u,
        .input_mode = 0,
        .status = 0u,
    };

    ::xcb_change_property(
        _connection,
        XCB_PROP_MODE_REPLACE,
        _window,
        motif_reply->atom,
        XCB_ATOM_INTEGER,
        32u,
        5u,
        &hints
    );

    free(motif_reply);
}

//==============================================================================
void X11Window::_acquire_multiuse_atoms() {
    auto wm_state_reply = ::xcb_intern_atom_reply(
        _connection,
        ::xcb_intern_atom(
            _connection,
            0,
            sizeof("_NET_WM_STATE") - 1,
            "_NET_WM_STATE"
        ),
        nullptr
    );

    auto fullscreen_reply = ::xcb_intern_atom_reply(
        _connection,
            ::xcb_intern_atom(
            _connection,
            0,
            sizeof("_NET_WM_STATE_FULLSCREEN") - 1,
            "_NET_WM_STATE_FULLSCREEN"
        ),
        nullptr
    );

    _wm_state_atom   = wm_state_reply->atom;
    _fullscreen_atom = fullscreen_reply->atom;

    free(wm_state_reply);
    free(fullscreen_reply);

    _fullscreen_event.response_type  = XCB_CLIENT_MESSAGE;
    _fullscreen_event.window         = _window;
    _fullscreen_event.type           = _wm_state_atom;
    _fullscreen_event.format         = 32u;
    _fullscreen_event.data.data32[0] = 2u;
    _fullscreen_event.data.data32[1] = _fullscreen_atom;
    _fullscreen_event.data.data32[2] = 0u;
}

//==============================================================================
void X11Window::_size_window(const uint32_t width, const uint32_t height) {
    CONSOLE_INFO("");

    _width  = width;
    _height = height;
   
    uint32_t value_mask = ::XCB_CONFIG_WINDOW_WIDTH |
                          ::XCB_CONFIG_WINDOW_HEIGHT;

    uint32_t value_list[] {
        _width,
        _height
    };

    ::xcb_configure_window(
        _connection,
        _window,
        value_mask,
        value_list
    );

    ::xcb_flush(_connection);
}

//==============================================================================
void X11Window::_center_window() {
    CONSOLE_INFO("");

    uint32_t pos_x = (_screen_width  / 2 - (_width  / 2));
    uint32_t pos_y = (_screen_height / 2 - (_height / 2));
    
    uint32_t value_mask = ::XCB_CONFIG_WINDOW_X |
                          ::XCB_CONFIG_WINDOW_Y;

    int32_t value_list[] {
        static_cast<int32_t>(pos_x) + _screen_x_offset,
        static_cast<int32_t>(pos_y) + _screen_y_offset,
    };

    ::xcb_configure_window(
        _connection,
        _window,
        value_mask,
        value_list
    );

    ::xcb_flush(_connection);
}

//==============================================================================
X11Window::X11Window(const vk::Instance &instance,
                     const uint32_t width, const uint32_t height) :
    _connection       { nullptr },
    _screen           { nullptr },
    _key_symbols      { nullptr },
    _window           { 0u },
    _delete_atom      { 0u },
    _wm_state_atom    { 0u },
    _fullscreen_atom  { 0u },
    _fullscreen_event { },
    _surface          { },
    _width            { width  },
    _height           { height },
    _screen_width     { 0u },
    _screen_height    { 0u },
    _screen_x_offset  { 0  },
    _screen_y_offset  { 0  },
    _fullscreen       { false },
    _running          { false },
    _instance         { instance }
{
    CONSOLE_INFO("");

    int screenp = 0;
    _connection = ::xcb_connect(nullptr, &screenp);

    if(::xcb_connection_has_error(_connection)) {
        CONSOLE_ERROR("Could not connect to X server.");
    }

    _key_symbols = xcb_key_symbols_alloc(_connection);

    ::xcb_screen_iterator_t screen_iter =
        ::xcb_setup_roots_iterator(::xcb_get_setup(_connection));

    for(int screen = screenp; screen > 0; --screen) {
        ::xcb_screen_next(&screen_iter);
    }

    _screen = screen_iter.data;
    _window = ::xcb_generate_id(_connection);

    _query_randr();

    if(_width == 0 || _height == 0) {
        _width  = static_cast<uint32_t>(_screen_width  * 0.75f);
        _height = static_cast<uint32_t>(_screen_height * 0.75f);
    }
}

X11Window::~X11Window() {
    CONSOLE_INFO("");
    ::vkDestroySurfaceKHR(_instance, _surface, nullptr);

    ::xcb_key_symbols_free(_key_symbols);
    ::xcb_destroy_window(_connection, _window);
    ::xcb_disconnect(_connection);
}

#endif // __linux__