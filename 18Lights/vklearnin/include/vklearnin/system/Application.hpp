#ifndef VKLEARNIN_SYSTEM_APPLICATION_HPP
#define VKLEARNIN_SYSTEM_APPLICATION_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct WindowCloseEvent;

class Application {
public:
    virtual void update()       = 0;
    virtual void submit_draws() = 0;
    virtual void init()         = 0;
    virtual void shutdown()     = 0;

    void run();

    void on_window_close(const WindowCloseEvent &event);

    Application();
    virtual ~Application();

    Application(Application &&) = delete;
    Application(const Application &) = delete;

    Application& operator=(Application &&) = delete;
    Application& operator=(const Application &) = delete;

private:
    bool _running;

};

} // namespace vkl

#endif // VKLEARNIN_SYSTEM_APPLICATION_HPP