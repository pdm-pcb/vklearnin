#include "vklearnin/vklearnin.hpp"
#include "vklearnin/platform/TargetWindow.hpp"

#include "vklearnin/platform/GLFWtoVKLKeys.hpp"

namespace vkl {

bool TargetWindow::_glfw_initialized { false };

// =============================================================================
bool TargetWindow::init() {
    if(_glfw_initialized) {
        Log::warn("GLFW already initialized.");
        return true;
    }

    if(::glfwInit() == 0) {
        Log::error("Failed to initialize GLFW");
        return false;
    }

    _glfw_initialized = true;
    Log::info("Initialized GLFW {:s}", ::glfwGetVersionString());

    ::glfwSetErrorCallback(TargetWindow::_error_callback);

    // Tell GLFW we'll handle the API
    ::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    return true;
}

// =============================================================================
bool TargetWindow::shutdown() {
    if(!_glfw_initialized) {
        Log::error("Cannot shut down GLFW before initializing it.");
        return false;
    }

    ::glfwTerminate();

    return true;
}

// =============================================================================
bool TargetWindow::create(std::string_view const app_name) {
    ::glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    ::glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    _window = ::glfwCreateWindow(
        320, 240,        // Default size that'll change immediately
        app_name.data(), // Window title/class/etc
        nullptr,         // Windowed mode
        nullptr          // No shared resources
    );

    if(_window == nullptr) {
        Log::error("GLFW window creation failed");
        return false;
    }

    ::glfwSetWindowSizeLimits(
        _window,
        320,            // minimum width
        240,            // minimum height
        GLFW_DONT_CARE, // maximum width
        GLFW_DONT_CARE  // maximum height
    );

    // For getting back to the window that triggered a given callback
    ::glfwSetWindowUserPointer(_window, this);

    // Input callbacks
    ::glfwSetKeyCallback(_window, TargetWindow::_key_callback);
    ::glfwSetCursorPosCallback(_window, TargetWindow::_mouse_move_callback);
    ::glfwSetWindowIconifyCallback(_window, TargetWindow::_iconify_callback);

    // Raw mouse input, then capture and hide the cursor
    ::glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    ::glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Adjust the size of the target window relative to the display it's on,
    // and center it
    if(!_get_resolution()) {
        destroy();
        return false;
    }
    _size_and_place();

    return true;
}

// =============================================================================
bool TargetWindow::destroy() {
    if(_window == nullptr) {
        Log::error("Must create target window before calling destroy.");
        return false;
    }

    ::glfwDestroyWindow(_window);

    return true;
}

// =============================================================================
bool TargetWindow::poll_events(InputStates &input_states) {
    ::glfwPollEvents();

    // If there's no change since last frame, the delta is zero
    _input_states.delta_x = static_cast<float>(_last_cursor_pos.x - _cursor_pos.x);
    _input_states.delta_y = static_cast<float>(_last_cursor_pos.y - _cursor_pos.y);
    _last_cursor_pos = _cursor_pos;

    input_states = _input_states;
    return static_cast<bool>(::glfwWindowShouldClose(_window));
}

// =============================================================================
bool TargetWindow::_get_resolution() {
    auto const *current_mode = ::glfwGetVideoMode(::glfwGetPrimaryMonitor());

    if(current_mode == nullptr) {
        Log::error("Could not get primary monitor's video modes.");
        return false;
    }

    auto const width = static_cast<float>(current_mode->width);
    auto const height = static_cast<float>(current_mode->height);

    _window_size = vk::Extent2D {
        .width  = static_cast<uint32_t>(width * 0.75f),
        .height = static_cast<uint32_t>(height * 0.75f)
    };

    _screen_center = vk::Offset2D {
        .x = static_cast<int32_t>(width * 0.5f),
        .y = static_cast<int32_t>(height * 0.5f)
    };

    return true;
}

// =============================================================================
void TargetWindow::_size_and_place() {
    ::glfwSetWindowSize(_window,
                        static_cast<int>(_window_size.width),
                        static_cast<int>(_window_size.height));

    auto const half_width  = static_cast<float>(_window_size.width)  * 0.5f;
    auto const half_height = static_cast<float>(_window_size.height) * 0.5f;

    _window_position = vk::Offset2D {
        .x = (static_cast<int32_t>(_screen_center.x)
              - static_cast<int32_t>(half_width)),
        .y = (static_cast<int32_t>(_screen_center.y)
              - static_cast<int32_t>(half_height)),
    };

    ::glfwSetWindowPos(_window,
                       static_cast<int32_t>(_window_position.x),
                       static_cast<int32_t>(_window_position.y));

    // Record initial cursor position
    ::glfwGetCursorPos(_window, &_cursor_pos.x, &_cursor_pos.y);
    ::glfwGetCursorPos(_window, &_last_cursor_pos.x, &_last_cursor_pos.y);
}

// =============================================================================
void TargetWindow::_error_callback(int code, char const *message) {
    Log::error("GLFW Error {}: '{:s}'", code, message);
}

// =============================================================================
void TargetWindow::_key_callback(GLFWwindow *window,
                                 int key, [[maybe_unused]] int scancode,
                                 int action,  [[maybe_unused]] int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
        ::glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    auto *target_window =
        static_cast<TargetWindow *>(::glfwGetWindowUserPointer(window));

    if(key == GLFW_KEY_W) target_window->_input_states.w_pressed = action;
    if(key == GLFW_KEY_A) target_window->_input_states.a_pressed = action;
    if(key == GLFW_KEY_S) target_window->_input_states.s_pressed = action;
    if(key == GLFW_KEY_D) target_window->_input_states.d_pressed = action;
    if(key == GLFW_KEY_Q) target_window->_input_states.q_pressed = action;
    if(key == GLFW_KEY_E) target_window->_input_states.e_pressed = action;

    if(key == GLFW_KEY_SPACE)        target_window->_input_states.space_pressed  = action;
    if(key == GLFW_KEY_LEFT_CONTROL) target_window->_input_states.lctrl_pressed  = action;
    if(key == GLFW_KEY_LEFT_SHIFT)   target_window->_input_states.lshift_pressed = action;

    if(key == GLFW_KEY_UP)    target_window->_input_states.up_pressed    = action;
    if(key == GLFW_KEY_DOWN)  target_window->_input_states.down_pressed  = action;
    if(key == GLFW_KEY_LEFT)  target_window->_input_states.left_pressed  = action;
    if(key == GLFW_KEY_RIGHT) target_window->_input_states.right_pressed = action;
}

// =============================================================================
void TargetWindow::_mouse_move_callback(GLFWwindow* window, double x, double y)
{
    auto *target_window =
        static_cast<TargetWindow *>(::glfwGetWindowUserPointer(window));

    target_window->_cursor_pos.x = x;
    target_window->_cursor_pos.y = y;
}

// =============================================================================
void TargetWindow::_mouse_button_callback([[maybe_unused]] GLFWwindow* window,
                                          [[maybe_unused]] int button,
                                          [[maybe_unused]] int action,
                                          [[maybe_unused]] int mods)
{ }

// =============================================================================
void TargetWindow::_iconify_callback(GLFWwindow* window, int iconified) {
    if(iconified == GLFW_TRUE) {
        Log::trace("Target window minimized.");

        int width = 0, height = 0;
        do {
            ::glfwWaitEvents();
            ::glfwGetFramebufferSize(window, &width, &height);
        } while(width == 0 || height == 0);

        Log::trace("Target window restored.");
    }
}

} // namespace vkl