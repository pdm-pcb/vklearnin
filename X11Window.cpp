#include "common.hpp"
#include "X11Window.hpp"

#include "Instance.hpp"

void X11Window::render_loop() {
    ::xcb_generic_event_t *event = nullptr;

    while((event = ::xcb_wait_for_event(_connection))) {
        switch(event->response_type & ~0x80) {
            case XCB_CLIENT_MESSAGE:
                auto *client_message =
                    reinterpret_cast<::xcb_client_message_event_t *>(event);
                if(client_message->data.data32[0] == _wm_delete) {
                    CONSOLE_WARN("Well, looks like we made it.");
                }
                break;
        }

        free(event);
    }

    ::xcb_destroy_window(_connection, _window);
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

    uint32_t event_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t value_list[] {
        _screen->black_pixel,
        0
    };

    ::xcb_create_window(
        _connection,
        XCB_COPY_FROM_PARENT,
        _window,
        _screen->root,
        0, 0, _x_res, _y_res, 0,
        ::XCB_WINDOW_CLASS_INPUT_OUTPUT,
        _screen->root_visual,
        event_mask,
        value_list
    );

    ::xcb_change_property(
        _connection,
        ::XCB_PROP_MODE_REPLACE,
        _window,
        ::XCB_ATOM_WM_NAME,
        ::XCB_ATOM_STRING,
        8u,
        strlen(APPLICATION_NAME),
        APPLICATION_NAME
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
}

void X11Window::init_surface() {
    ::VkXcbSurfaceCreateInfoKHR surface_info { };
    surface_info.sType = ::VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    surface_info.connection = _connection;
    surface_info.window = _window;

    ::VkResult result = vkCreateXcbSurfaceKHR(
        instance.vulkan_instance(),
        &surface_info,
        nullptr,
        &_surface
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_CRITICAL("Unable to create XCB surface.");
    }
}

X11Window::X11Window(const uint32_t x_res, const uint32_t y_res,
                     const Instance &instance) :
    _instance   { instance },
    _x_res      { x_res },
    _y_res      { y_res },
    _connection { nullptr },
    _window     { 0 },
    _screen     { nullptr },
    _wm_delete  { 0 },
    _wm_proto   { 0 },
    _surface    { nullptr }
{
    CONSOLE_INFO("");
}

X11Window::~X11Window() {
    CONSOLE_INFO("");
    ::vkDestroySurfaceKHR(instance.vulkan_instance(), _surface, nullptr);
}