#ifndef VKLEARNIN_ENGINE_ENGINE_HPP
#define VKLEARNIN_ENGINE_ENGINE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/shaders/CameraData.hpp"

namespace vkl {

class Application;
class Swapchain;
class Pipeline;

class Engine final {
public:
    void render_loop();

    void init();
    void shutdown();

    explicit Engine(Application &app);
    ~Engine();

    Engine() = delete;

    Engine(Engine &&) = delete;
    Engine(const Engine &) = delete;

    Engine & operator=(Engine &&) = delete;
    Engine & operator=(const Engine &) = delete;

private:
    Swapchain *_swapchain;
    std::vector<Pipeline *> _pipelines;

    uint32_t _frame_index;

    Application &_application;

    void _image_invalid();
    void _next_frame();
};

} // namespace vkl

#endif // VKLEARNIN_ENGINE_ENGINE_HPP