#ifndef VKLEARNIN_SYSTEM_ENGINE_HPP
#define VKLEARNIN_SYSTEM_ENGINE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/system/devices/CmdPool.hpp"
#include "vklearnin/system/devices/CmdBuffer.hpp"
#include "vklearnin/rendering/Renderer.hpp"
#include "vklearnin/rendering/Camera.hpp"

namespace vkl {

class Application;

class Engine final {
public:
    void render_loop();

    void init();
    void shutdown();

    explicit Engine(Application &app);
    ~Engine() = default;

    Engine() = delete;

    Engine(Engine &&) = delete;
    Engine(const Engine &) = delete;

    Engine& operator=(Engine &&) = delete;
    Engine& operator=(const Engine &) = delete;

private:
    Application &_application;

    std::vector<CmdPool>   _cmd_pools;
    std::vector<CmdBuffer> _cmd_buffers;

    Renderer _renderer;
    Camera   _persp_cam;
};

} // namespace vkl

#endif // VKLEARNIN_SYSTEM_ENGINE_HPP