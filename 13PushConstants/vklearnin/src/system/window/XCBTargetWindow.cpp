#ifdef VKL_LINUX

#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/window/XCBTargetWindow.hpp"

namespace vkl {

bool                 XCBTargetWindow::_carry_on      = true;
vk::SurfaceKHR       XCBTargetWindow::_surface       { };
::xcb_connection_t  *XCBTargetWindow::_connection    = nullptr;
::xcb_screen_t      *XCBTargetWindow::_screen        = nullptr;
::xcb_key_symbols_t *XCBTargetWindow::_key_symbols   = nullptr;
::xcb_window_t       XCBTargetWindow::_window        = 0u;
::xcb_atom_t         XCBTargetWindow::_delete_atom   = 0u;
::xcb_atom_t         XCBTargetWindow::_wm_state_atom = 0u;

XCBTargetWindow::CenterPos XCBTargetWindow::_center;

using client_msg        = ::xcb_client_message_event_t *;
using config_notify     = ::xcb_configure_notify_event_t *;
using keypress_notify   = ::xcb_key_press_event_t *;
using keyrelease_notify = ::xcb_key_release_event_t *;
using property_notify   = ::xcb_property_notify_event_t *;

static constexpr uint32_t XCB_EVENT_RESPONSE_TYPE_MASK = ~0x80u;

//==============================================================================
bool XCBTargetWindow::message_loop() {
    ::xcb_generic_event_t *event = nullptr;

    while((event = ::xcb_poll_for_event(_connection))) {       
        uint32_t event_type =
            event->response_type & XCB_EVENT_RESPONSE_TYPE_MASK;
        switch(event_type) {
            case 0:
                break;

            case XCB_KEY_PRESS: {
                auto *press = reinterpret_cast<keypress_notify>(event);
                auto key = ::xcb_key_symbols_get_keysym(
                    _key_symbols,
                    press->detail,
                    0
                );

                switch(key) {
                    case XK_Escape:
                        _carry_on = false;
                        break;
                }
                break;
            }

            case XCB_CONFIGURE_NOTIFY:  {
                auto *config = reinterpret_cast<config_notify>(event);
                if(config->width  != RenderConfig::window_width ||
                   config->height != RenderConfig::window_height) {
                    RenderConfig::window_width  = config->width;
                    RenderConfig::window_height = config->height;

                    RenderConfig::window_pos_x = RenderConfig::screen_width / 2;
                    RenderConfig::window_pos_x -= RenderConfig::window_width / 2;

                    RenderConfig::window_pos_y = RenderConfig::screen_height / 2;
                    RenderConfig::window_pos_y -= RenderConfig::window_height / 2;

                    _size_and_place();
                }
                break;
            }
        }

        free(event);
    }

    return _carry_on;
}

//==============================================================================
void XCBTargetWindow::spawn_window(const uint32_t width, const uint32_t height,
                                   const int32_t  pos_x, const int32_t  pos_y)
{
    _init();
    
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

    // If width and height aren't provided by Application, then just opt for
    // 75% of the available real estate
    if(width == 0u || height == 0u) {
        RenderConfig::window_width  = 
            static_cast<uint32_t>(RenderConfig::screen_width * 0.75f);
        RenderConfig::window_height = 
            static_cast<uint32_t>(RenderConfig::screen_height * 0.75f);
    }
    else {
        RenderConfig::window_width = width;
        RenderConfig::window_height = height;
    }

    // Determine the window's eventual position on screen
    int32_t half_width  = RenderConfig::window_width  / 2;
    int32_t half_height = RenderConfig::window_height / 2;
    if(pos_x == 0 || pos_y == 0) {
        RenderConfig::window_pos_x = _center.x - half_width;
        RenderConfig::window_pos_y = _center.y - half_height;
    }
    else {
        RenderConfig::window_pos_x = pos_x;
        RenderConfig::window_pos_y = pos_y;
    }

    ::xcb_create_window(
        _connection,
        XCB_COPY_FROM_PARENT,
        _window,
        _screen->root,
        RenderConfig::window_pos_x,
        RenderConfig::window_pos_y,
        RenderConfig::window_width,
        RenderConfig::window_height,
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

    _size_and_place();
}

//==============================================================================
void XCBTargetWindow::create_surface() {
    // The details Vulkan cares about
    vk::XcbSurfaceCreateInfoKHR surface_info {
        .connection = _connection,
        .window = _window,
    };

    // Create, check, assign
    auto result = GraphicsAPI::native().createXcbSurfaceKHR(surface_info);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Unable to create XCB KHR surface: '{}'",
            to_string(result.result)
        );
    }
    _surface = result.value;

    CONSOLE_TRACE(
        "Created Vulkan surface {:#x}",
        reinterpret_cast<uint64_t>(::VkSurfaceKHR(_surface))
    );
}

//==============================================================================
void XCBTargetWindow::destroy_surface() {
    CONSOLE_TRACE(
        "Destroying Vulkan surface {:#x}",
        reinterpret_cast<uint64_t>(::VkSurfaceKHR(_surface))
    );
    GraphicsAPI::native().destroy(_surface);

    ::xcb_key_symbols_free(_key_symbols);
    ::xcb_destroy_window(_connection, _window);
    ::xcb_disconnect(_connection);
}

//==============================================================================
void XCBTargetWindow::_init() {
    int screenp = 0;
    _connection = ::xcb_connect(nullptr, &screenp);

    if(::xcb_connection_has_error(_connection)) {
        CONSOLE_CRITICAL("Could not connect to X server.");
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
}

//==============================================================================
void XCBTargetWindow::_query_randr() {
    
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
    CONSOLE_TRACE("Found {} {}", iter.rem,
                  (iter.rem == 1 ? "monitor" : "monitors"));

    while(iter.rem > 0) {
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
            RenderConfig::screen_width    = info->width;
            RenderConfig::screen_height   = info->height;
            RenderConfig::screen_x_offset = info->x;
            RenderConfig::screen_y_offset = info->y;
        }

        ::xcb_randr_monitor_info_next(&iter);
    }

    free(reply);

    _center.x = (RenderConfig::screen_width / 2);
    _center.y = (RenderConfig::screen_height / 2);
}

//==============================================================================
void XCBTargetWindow::_redirect_delete() {
    
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
void XCBTargetWindow::_remove_decorations() {
    
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
void XCBTargetWindow::_acquire_multiuse_atoms() {
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

    _wm_state_atom   = wm_state_reply->atom;

    free(wm_state_reply);
}

//==============================================================================
void XCBTargetWindow::_size_and_place() {
    uint32_t value_mask = ::XCB_CONFIG_WINDOW_X |
                          ::XCB_CONFIG_WINDOW_Y |
                          ::XCB_CONFIG_WINDOW_WIDTH |
                          ::XCB_CONFIG_WINDOW_HEIGHT;

    int32_t value_list[] {
        RenderConfig::window_pos_x + RenderConfig::screen_x_offset,
        RenderConfig::window_pos_y + RenderConfig::screen_y_offset,
        static_cast<int32_t>(RenderConfig::window_width),
        static_cast<int32_t>(RenderConfig::window_height)
    };


    CONSOLE_TRACE(
        "Window size: {}x{}, position: {}x{}",
        value_list[2], value_list[3],
        value_list[0], value_list[1]
    );

    ::xcb_configure_window(
        _connection,
        _window,
        value_mask,
        value_list
    );

    ::xcb_flush(_connection);
}

} // namespace vkl

#endif // VKL_LINUX