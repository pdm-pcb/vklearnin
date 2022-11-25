#ifndef VKLEARNIN_SYSTEM_APPLICATION_HPP
#define VKLEARNIN_SYSTEM_APPLICATION_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Engine;
class Pipeline;
class Swapchain;

class Application {
public:
    void run();

    virtual std::vector<Pipeline *>
    create_pipelines(const Swapchain &swapchain) = 0;

    virtual const vk::CommandBuffer &
    execute_pipelines(const float time_delta, const uint32_t frame_index) = 0;

    virtual void swapchain_image_invalid() = 0;

    virtual void init() = 0;
    virtual void shutdown() = 0;

    Application();
    virtual ~Application();

    Application(Application &&) = delete;
    Application(const Application &) = delete;

    Application & operator=(Application &&) = delete;
    Application & operator=(const Application &) = delete;

private:
    bool    _running;
    Engine *_engine;
};

} // namespace vkl
#endif // VKLEARNIN_SYSTEM_APPLICATION_HPP