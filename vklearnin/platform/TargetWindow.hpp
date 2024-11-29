#ifndef VKLEARNIN_PLATFORM_TARGETWINDOW_HPP
#define VKLEARNIN_PLATFORM_TARGETWINDOW_HPP

#include "vklearnin/pch.hpp"

#ifdef VKL_LINUX
    #define GLFW_EXPOSE_NATIVE_X11
#elif VKL_WINDOWS
    #define GLFW_EXPOSE_NATIVE_WIN32
#endif // VKL platform

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace vkl {

class TargetWindow final {
public:
    TargetWindow() = default;
    ~TargetWindow() = default;

    TargetWindow(TargetWindow &&) = delete;
    TargetWindow(TargetWindow const &) = delete;

    TargetWindow & operator=(TargetWindow &&) = delete;
    TargetWindow & operator=(TargetWindow const &) = delete;

    static bool init();
    static bool shutdown();

    bool create(std::string_view const app_name);
    bool destroy();

    inline void show() const { ::glfwShowWindow(_window); }
    inline void hide() const { ::glfwHideWindow(_window); }

    [[nodiscard]] bool poll_events();

#ifdef VKL_LINUX
    [[nodiscard]] inline auto display() const { return ::glfwGetX11Display(); }
    [[nodiscard]] inline auto native() const {
        return ::glfwGetX11Window(_window);
    }
#elif VKL_WINDOWS
    [[nodiscard]] inline auto native() const {
        return ::glfwGetWin32Window(_window);
    }
#endif // VKL platform

private:
    static bool _initialized;

    GLFWwindow *_window { nullptr };

    vk::Offset2D _screen_center   { };
    vk::Extent2D _window_size     { };
    vk::Offset2D _window_position { };

    void _get_resolution();
    void _size_and_place();

    static void _error_callback(int code, char const *message);

    static void _key_callback(GLFWwindow *window, int key, int scancode,
                              int action, int mods);
    static void _mouse_move_callback(GLFWwindow* window, double x, double y);
    static void _mouse_button_callback(GLFWwindow* window, int button,
                                       int action, int mods);
    static void _iconify_callback(GLFWwindow* window, int iconified);

};

} // namespace vkl

#endif // VKLEARNIN_PLATFORM_TARGETWINDOW_HPP