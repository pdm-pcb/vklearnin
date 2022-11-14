#ifndef VKLEARNIN_ENGINE_ENGINE_HPP
#define VKLEARNIN_ENGINE_ENGINE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/engine/Framebuffer.hpp"
#include "vklearnin/engine/CameraData.hpp"

namespace vkl {

class Swapchain;
class Pipeline;

////////////////////////////////////////////////////////////////////////////////
// TODO: replace with proper asset management
class XZPlane;
////////////////////////////////////////////////////////////////////////////////

class Engine final {
public:
    using FBList = std::vector<Framebuffer>;

    void render_loop();

    void init();
    void shutdown();

    Engine();
    ~Engine();

    Engine(Engine &&) = delete;
    Engine(const Engine &) = delete;

    Engine & operator=(Engine &&) = delete;
    Engine & operator=(const Engine &) = delete;

private:
    Swapchain *_swapchain;
    Pipeline  *_pipeline;

    FBList   _framebuffers;
    uint32_t _current_framebuffer;

    CameraData _camera_data;

    XZPlane *_xzplane;

    void _create_framebuffers();
    void _destroy_framebuffers();
    void _image_invalid();
    void _next_framebuffer();
};

} // namespace vkl

#endif // VKLEARNIN_ENGINE_ENGINE_HPP