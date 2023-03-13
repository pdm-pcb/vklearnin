#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/window/TargetWindow.hpp"

#include "vklearnin/system/GraphicsAPI.hpp"

namespace vkl {

TargetWindow::ScreenPos TargetWindow::_center { };

vk::SurfaceKHR TargetWindow::_surface { };

::SDL_Window *TargetWindow::_window { nullptr };

//==============================================================================
void TargetWindow::message_loop() {
    ::SDL_Event event;
    while(::SDL_PollEvent(&event)) {
        switch(event.type) {
            case ::SDL_KEYDOWN:
                if(event.key.keysym.sym == ::SDLK_ESCAPE) {
                    ::SDL_Event quit_event { };
                    quit_event.type = ::SDL_QUIT;

                    ::SDL_PushEvent(&quit_event);
                }

                EventBroker::emit<KeyPressEvent>(
                    sdl_to_vkl(event.key.keysym.scancode)
                );
                break;

            case ::SDL_KEYUP:
                EventBroker::emit<KeyReleaseEvent>(
                    sdl_to_vkl(event.key.keysym.scancode)
                );
                break;

            case ::SDL_MOUSEMOTION:
                EventBroker::emit<MouseMoveEvent>(
                    event.motion.xrel,
                    event.motion.yrel
                );
                break;

            case ::SDL_MOUSEBUTTONDOWN:
                EventBroker::emit<MouseButtonPressEvent>(
                    sdl_to_vkl(event.button.button)
                );
                break;

            case ::SDL_MOUSEBUTTONUP:
                EventBroker::emit<MouseButtonReleaseEvent>(
                    sdl_to_vkl(event.button.button)
                );
                break;

            case ::SDL_MOUSEWHEEL:
                EventBroker::emit<MouseScrollEvent>(
                    event.wheel.y,
                    event.wheel.x
                );
                break;

            case ::SDL_QUIT:
                EventBroker::emit<WindowCloseEvent>();
                _free_cursor();
                break;

            case ::SDL_WINDOWEVENT: {
                switch(event.window.event) {
                    case ::SDL_WINDOWEVENT_FOCUS_GAINED:
                        _trap_cursor();
                        break;

                    case ::SDL_WINDOWEVENT_FOCUS_LOST:
                        _free_cursor();
                        break;
                }
                break;
            }

        }
    }
}

//==============================================================================
void TargetWindow::init() {
    auto const init_result = ::SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    if(init_result != 0) {
        CONSOLE_CRITICAL("Failed to initialize SDL2: '{}'", ::SDL_GetError());
        return;
    }

    ::SDL_DisplayMode display_mode { };
    auto const dsp_result = ::SDL_GetCurrentDisplayMode(0, &display_mode);

    if(dsp_result != 0) {
        CONSOLE_CRITICAL(
            "SDL2 could not query display mode: '{}'",
            ::SDL_GetError()
        );
        return;
    }

    ::SDL_version ver { };
    ::SDL_GetVersion(&ver);
    CONSOLE_INFO("SDL {}.{}.{} initialized", ver.major, ver.minor, ver.patch);

    RenderConfig::screen_width  = display_mode.w;
    RenderConfig::screen_height = display_mode.h;

    _center.x = RenderConfig::screen_width / 2;
    _center.y = RenderConfig::screen_width / 2;
}

//==============================================================================
void TargetWindow::shutdown() {
    ::SDL_DestroyWindow(_window);
    _window = nullptr;
    ::SDL_Quit();
}

//==============================================================================
void TargetWindow::spawn_window(uint32_t const width, uint32_t const height,
                                int32_t  const pos_x, int32_t  const pos_y)
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

    _window = SDL_CreateWindow(
        APP_NAME,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        RenderConfig::window_width,
        RenderConfig::window_height,
        ::SDL_WINDOW_SHOWN
        | ::SDL_WINDOW_VULKAN
        | ::SDL_WINDOW_BORDERLESS
    );

    if(_window == nullptr) {
        CONSOLE_CRITICAL(
            "Unable to create SDL2 window: '{}'",
            ::SDL_GetError()
        );
        return;
    }

    _size_and_place();
}

//==============================================================================
void TargetWindow::create_surface() {
    auto const result = ::SDL_Vulkan_CreateSurface(
        _window,
        GraphicsAPI::native(),
        reinterpret_cast<VkSurfaceKHR *>(&_surface)
    );

    if(result != ::SDL_TRUE) {
        CONSOLE_CRITICAL(
            "Could not create Vulkan surface via SDL2: '{}'",
            ::SDL_GetError()
        );
        return;
    }

    CONSOLE_TRACE(
        "Created Vulkan surface {:#x}",
        reinterpret_cast<uint64_t>(::VkSurfaceKHR(_surface))
    );
}

//==============================================================================
void TargetWindow::destroy_surface() {
    CONSOLE_TRACE(
        "Destroying Vulkan surface {:#x}",
        reinterpret_cast<uint64_t>(::VkSurfaceKHR(_surface))
    );
    GraphicsAPI::native().destroy(_surface);
}

//==============================================================================
void TargetWindow::_size_and_place() {
    CONSOLE_TRACE(
        "Window size: {}x{}, position: {}x{}",
        RenderConfig::window_width, RenderConfig::window_height,
        RenderConfig::window_pos_x, RenderConfig::window_pos_y
    );

    // Update the window aspect ratio
    RenderConfig::window_aspect =
        static_cast<float>(RenderConfig::window_width) /
        static_cast<float>(RenderConfig::window_height);
}

//==============================================================================
void TargetWindow::_trap_cursor() {
    auto const relative_result = ::SDL_SetRelativeMouseMode(::SDL_TRUE);
    if(relative_result != 0) {
        CONSOLE_ERROR(
            "SDL2 could not enable relative mouse mode: '{}'",
            ::SDL_GetError()
        );
    }
}

//==============================================================================
void TargetWindow::_free_cursor() {
    auto const relative_result = ::SDL_SetRelativeMouseMode(::SDL_FALSE);
    if(relative_result != 0) {
        CONSOLE_ERROR(
            "SDL2 could not disable relative mouse mode: '{}'",
            ::SDL_GetError()
        );
    }
}

} // namespace vkl