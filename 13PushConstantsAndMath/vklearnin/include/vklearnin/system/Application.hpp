#ifndef VKLEARNIN_SYSTEM_APPLICATION_HPP
#define VKLEARNIN_SYSTEM_APPLICATION_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/Framebuffer.hpp"
#include "vklearnin/rendering/CameraData.hpp"

namespace vkl {

class Swapchain;
class Pipeline;

class Application {
public:
    void init();
    void run();

    Application();
    ~Application();

    Application(Application &&) = delete;
    Application(const Application &) = delete;

    Application & operator=(Application &&) = delete;
    Application & operator=(const Application &) = delete;

private:
    bool _running;

    Swapchain *_swapchain;
    Pipeline  *_pipeline;

    std::vector<Framebuffer> _framebuffers;
    uint32_t                 _current_framebuffer;

    CameraData _camera_data;

    void _image_invalid();
};

} // namespace vkl
#endif // VKLEARNIN_SYSTEM_APPLICATION_HPP