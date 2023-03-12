#ifdef VKL_LINUX

#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/window/XCBTargetWindow.hpp"

#include "vklearnin/system/GraphicsAPI.hpp"

namespace vkl {

XCBTargetWindow::ScreenPos XCBTargetWindow::_center;

vk::SurfaceKHR XCBTargetWindow::_surface { };

::xcb_connection_t  *XCBTargetWindow::_connection    = nullptr;
::xcb_screen_t      *XCBTargetWindow::_screen        = nullptr;
::xcb_key_symbols_t *XCBTargetWindow::_key_symbols   = nullptr;
::xcb_window_t       XCBTargetWindow::_window        = 0u;
::xcb_atom_t         XCBTargetWindow::_delete_atom   = 0u;
::xcb_atom_t         XCBTargetWindow::_wm_state_atom = 0u;

static constexpr uint32_t XCB_EVENT_RESPONSE_TYPE_MASK = ~0x80u;

using client_msg     = ::xcb_client_message_event_t *;
using config_event   = ::xcb_configure_notify_event_t *;
using key_press      = ::xcb_key_press_event_t *;
using key_release    = ::xcb_key_release_event_t *;
using button_press   = ::xcb_button_press_event_t *;
using button_release = ::xcb_button_release_event_t *;
using motion_event   = ::xcb_motion_notify_event_t *;

//==============================================================================
void XCBTargetWindow::message_loop() {
    ::xcb_generic_event_t *event = nullptr;

    while((event = ::xcb_poll_for_event(_connection))) {
        uint32_t event_type = event->response_type &
                              XCB_EVENT_RESPONSE_TYPE_MASK;

        switch(event_type) {
            case XCB_KEY_PRESS: {
                auto *press = reinterpret_cast<key_press>(event);
                auto key = ::xcb_key_symbols_get_keysym(
                    _key_symbols,
                    press->detail,
                    0
                );

                EventBroker::emit<KeyPressEvent>(xcb_to_vkl(key));

                if(key == XK_Escape) {
                    EventBroker::emit<WindowCloseEvent>();
                }

                break;
            }

            case XCB_KEY_RELEASE: {
                auto *release = reinterpret_cast<key_release>(event);
                auto key = ::xcb_key_symbols_get_keysym(
                    _key_symbols,
                    release->detail,
                    0
                );

                EventBroker::emit<KeyReleaseEvent>(xcb_to_vkl(key));
                break;
            }

            case XCB_BUTTON_PRESS: {
                auto *press = reinterpret_cast<button_press>(event);
                EventBroker::emit<MouseButtonPressEvent>(
                    xcb_to_vkl(press->detail)
                );

                break;
            }

            case XCB_BUTTON_RELEASE: {
                auto *release = reinterpret_cast<button_release>(event);
                EventBroker::emit<MouseButtonReleaseEvent>(
                    xcb_to_vkl(release->detail)
                );

                break;
            }

            case XCB_MOTION_NOTIFY: {
                auto *motion = reinterpret_cast<motion_event>(event);
                CONSOLE_ERROR(
                    "{}x{}, {}x{}",
                    motion->root_x, motion->root_y,
                    motion->event_x, motion->event_y
                );
                
                break;
            }

            case XCB_CONFIGURE_NOTIFY:  {
                auto *config = reinterpret_cast<config_event>(event);
                if(config->width  != RenderConfig::window_width ||
                   config->height != RenderConfig::window_height) {
                    RenderConfig::window_width  = config->width;
                    RenderConfig::window_height = config->height;

                    auto half_width =
                        static_cast<int32_t>(RenderConfig::window_width)  / 2;
                    auto half_height =
                        static_cast<int32_t>(RenderConfig::window_height) / 2;

                    RenderConfig::window_pos_x = _center.x - half_width;
                    RenderConfig::window_pos_y = _center.y - half_height;

                    _size_and_place();
                }
                break;
            }

            default: break;
        }

        free(event);
    }
}

//==============================================================================
void XCBTargetWindow::init() {
    int screen_ptr = 0;
    _connection = ::xcb_connect(nullptr, &screen_ptr);

    if(::xcb_connection_has_error(_connection)) {
        CONSOLE_CRITICAL("Could not connect to X server.");
        return;
    }

    _key_symbols = ::xcb_key_symbols_alloc(_connection);

    auto screen_iter =
        ::xcb_setup_roots_iterator(::xcb_get_setup(_connection));

    for(int screen = screen_ptr; screen > 0; --screen) {
        ::xcb_screen_next(&screen_iter);
    }

    _screen = screen_iter.data;
    _window = ::xcb_generate_id(_connection);

    _query_randr();
}

//==============================================================================
void XCBTargetWindow::shutdown() {
    // lolwut
}

//==============================================================================
void XCBTargetWindow::spawn_window(uint32_t const width, uint32_t const height,
                                   int32_t const  pos_x, int32_t const  pos_y)
{
    // If width and height aren't provided by Application, then just opt for
    // two-thirds of the available real estate
    if(width == 0u || height == 0u) {
        auto width_fraction  = static_cast<float>(RenderConfig::screen_width);
        auto height_fraction = static_cast<float>(RenderConfig::screen_height);
        width_fraction  *= 0.75f;
        height_fraction *= 0.75f;

        RenderConfig::window_width  = static_cast<uint32_t>(width_fraction);
        RenderConfig::window_height = static_cast<uint32_t>(height_fraction);
    }
    else {
        RenderConfig::window_width = width;
        RenderConfig::window_height = height;
    }

    // Determine the window's eventual position on screen
    auto half_width  = static_cast<int32_t>(RenderConfig::window_width)  / 2;
    auto half_height = static_cast<int32_t>(RenderConfig::window_height) / 2;
    if(pos_x == 0 || pos_y == 0) {
        RenderConfig::window_pos_x = _center.x - half_width;
        RenderConfig::window_pos_y = _center.y - half_height;
    }
    else {
        RenderConfig::window_pos_x = pos_x;
        RenderConfig::window_pos_y = pos_y;
    }

    auto const vakue_mask = ::XCB_CW_BACK_PIXEL | ::XCB_CW_EVENT_MASK;
    uint32_t const value_list[] {
        _screen->black_pixel,
        ::XCB_EVENT_MASK_KEY_PRESS |
        ::XCB_EVENT_MASK_KEY_RELEASE |
        ::XCB_EVENT_MASK_BUTTON_PRESS |
        ::XCB_EVENT_MASK_BUTTON_RELEASE |
        ::XCB_EVENT_MASK_POINTER_MOTION |
        ::XCB_EVENT_MASK_BUTTON_MOTION |
        ::XCB_EVENT_MASK_EXPOSURE |
        ::XCB_EVENT_MASK_STRUCTURE_NOTIFY
    };

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
    _register_input();

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
        .window     = _window,
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
void XCBTargetWindow::_query_randr() {

    auto *reply = ::xcb_randr_get_monitors_reply(
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

    ::free(reply);

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

    ::free(delete_reply);
    ::free(protocols_reply);
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

// =============================================================================
void XCBTargetWindow::_register_input() {
    // First, just check for the extension
    auto const *xi_name = "XInputExtension";
    auto const xi_cookie =
        ::xcb_query_extension(_connection, ::strlen(xi_name), xi_name);
	auto *xi_reply =
        ::xcb_query_extension_reply(_connection, xi_cookie, nullptr);

	if(xi_reply == nullptr) {
        CONSOLE_CRITICAL("No XInput extension query reply");
        return;
    }
    ::free(xi_reply);

    // Next, check for the version we want
    auto const xi_ver_cookie =
        ::xcb_input_xi_query_version(_connection, 2u, 4u);
    auto *xi_ver_reply =
        ::xcb_input_xi_query_version_reply(_connection, xi_ver_cookie, nullptr);

    if(xi_ver_reply == nullptr) {
        CONSOLE_CRITICAL("No XInput version query reply");
        return;
    }
    ::free(xi_ver_reply);

    // auto const grab_cookie =
    //     ::xcb_input_xi_grab_device(
    //         _connection,
    //         _window,
    //         0u, // time
    //         0u, // cursor

    //     )
}

//==============================================================================
void XCBTargetWindow::_size_and_place() {
    CONSOLE_TRACE(
        "Window size: {}x{}, position: {}x{}",
        RenderConfig::window_width, RenderConfig::window_height,
        RenderConfig::window_pos_x, RenderConfig::window_pos_y
    );

    // Update the window aspect ratio
    RenderConfig::window_aspect =
        static_cast<float>(RenderConfig::window_width) /
        static_cast<float>(RenderConfig::window_height);

    

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