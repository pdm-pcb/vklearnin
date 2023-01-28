## Integrating the new Window
The first step to leveraging the new class will be to decouple the rest of the code from anything platform-specific. Instead of runtime polymorphism, I've chosen to make this decision  at compile time via the following "unifying" header.

```cpp
#ifndef VKLEARNIN_SYSTEM_TARGETWINDOW_HPP
#define VKLEARNIN_SYSTEM_TARGETWINDOW_HPP

#include "vklearnin/system/Win32TargetWindow.hpp"
#include "vklearnin/system/XCBTargetWindow.hpp"

namespace vkl {

#if defined(VKL_LINUX)
    using TargetWindow = XCBTargetWindow;
#elif defined(VKL_WINDOWS)
    using TargetWindow = Win32TargetWindow;
#endif

} // namespace vkl
#endif // VKLEARNIN_SYSTEM_TARGETWINDOW_HPP
```

Let's test the new capabilities of the code base. Include the new header in `Application` and add one new line to `Application::_init()`.

```cpp
void Application::_init() {
    ConsoleLog::init();
    VKInstance::init();
    TargetWindow::spawn_window();
}
```

The only other prerequisite for initial testing is to call `TargetWindow::message_loop()` from within `Application::run()`. While we've been simply running the application up until this point, this main path of ours will now become a loop which can only be ended by tapping the escape key.

```cpp
void Application::run() {
    _engine->init();

    while(TargetWindow::message_loop()) {
        Timekeeper::frame_start();
            _engine->render_loop();
        Timekeeper::frame_end();
        CONSOLE_TRACE("Sixty frames might take: {}", Timekeeper::frametime());
    }

    _engine->shutdown();
}
```

As written, the code should now run and a native window will pop up with the size and in the position specified. Pressing escape will cause the window to close itself and the whole program to clean up after itself. The only catch here is that we're still sleeping for one second within `Engine::render_loop()`, so there'll be a proportionate delay between when you hit escape and when the program actually notices. This is annoying, but we'll fix it before long. If you can't wait, feel free to modify it and find your own acceptable stopgap. =)

## Testing the Surface
Testing the surface could've been a step of the above test, but I heart iteration and you can too.

This time we add a line to `Application::_init()`  as well as `_shutdown()`.

```cpp
// =============================================================================
void Application::_init() {
    ConsoleLog::init();
    VKInstance::init();
    TargetWindow::spawn_window();
    TargetWindow::create_surface();
}

// =============================================================================
void Application::_shutdown() {
    TargetWindow::destroy_surface();
    VKInstance::shutdown();
}
```

All together, this code should produce identical visual output to the above. The changes will be noted in the console log statements, though. For me, it looks like this:

![[Pasted image 20230108033049.png]]

