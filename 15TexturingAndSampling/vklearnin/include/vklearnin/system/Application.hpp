#ifndef VKLEARNIN_SYSTEM_APPLICATION_HPP
#define VKLEARNIN_SYSTEM_APPLICATION_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Engine;

class Application {
public:
    void run();

    Application();
    virtual ~Application();

    Application(Application &&) = delete;
    Application(const Application &) = delete;

    Application & operator=(Application &&) = delete;
    Application & operator=(const Application &) = delete;

private:
    bool _running;
    Engine *_engine;
};

} // namespace vkl
#endif // VKLEARNIN_SYSTEM_APPLICATION_HPP