#ifndef VKLEARNIN_SYSTEM_ENGINE_HPP
#define VKLEARNIN_SYSTEM_ENGINE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Application;

class Engine final {
public:
    void render_loop();

    explicit Engine(Application &app);
    ~Engine() = default;

    Engine() = delete;

    Engine(Engine &&) = delete;
    Engine(const Engine &) = delete;

    Engine & operator=(Engine &&) = delete;
    Engine & operator=(const Engine &) = delete;

private:
    Application &_application;
};

} // namespace vkl

#endif // VKLEARNIN_SYSTEM_ENGINE_HPP