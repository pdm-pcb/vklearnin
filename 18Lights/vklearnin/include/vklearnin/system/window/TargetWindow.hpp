#ifndef VKLEARNIN_SYSTEM_WINDOW_TARGETWINDOW_HPP
#define VKLEARNIN_SYSTEM_WINDOW_TARGETWINDOW_HPP

namespace vkl {

class TargetWindow {
public:
    // Give the OS a moment to speak up
    static void message_loop();

    static void init();
    static void shutdown();

    // Spawn (and size and place?) a native window
    static void spawn_window(uint32_t const width  = 0u,
                             uint32_t const height = 0u,
                             int32_t  const pos_x  = 0u,
                             int32_t  const pos_y  = 0u);

    // Manage the Vulkan surface
    static void create_surface();
    static void destroy_surface();

    inline static auto const& surface() { return _surface; }

    // Only one target window at a time

    TargetWindow() = delete;

private:
    // Helps with centering the window on screen
    static struct ScreenPos {
        int32_t x = 0;
        int32_t y = 0;
    } _center;

    // Vulkan specifics
    static vk::SurfaceKHR _surface;

    // SDL specifics
    static ::SDL_Window *_window;

    // Make it just right
    static void _size_and_place();

    // Eventt handlers for window focus
    static void _focus_gained();
    static void _focus_lost();

    // Branchless mouse movement handling
    static void (*_handle_mouse_move)(int x, int y);

    inline static void _handle_mouse_in_focus(int x, int y) {
        EventBroker::emit<MouseMoveEvent>(x, y);
    }

    inline static void _handle_mouse_out_of_focus([[maybe_unused]] int x,
                                                  [[maybe_unused]] int y)
    { }
};

} // namespace vkl

#endif // VKLEARNIN_SYSTEM_WINDOW_TARGETWINDOW_HPP