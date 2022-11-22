#ifndef VKLEARNIN_ENGINE_ENGINE_HPP
#define VKLEARNIN_ENGINE_ENGINE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/shaders/CameraData.hpp"

namespace vkl {

class Swapchain;
class Pipeline;
class FrameData;

////////////////////////////////////////////////////////////////////////////////
// TODO: replace with proper asset management
class XZUnitPlane;
class UnitCube;
////////////////////////////////////////////////////////////////////////////////

class Engine final {
public:
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

    std::vector<FrameData> _frames;
    uint32_t _frame_index;

    CameraData _camera_data;

    XZUnitPlane *_xz_unit_plane;
    UnitCube    *_unit_cube;

    void _create_frames();
    void _destroy_frames();
    void _image_invalid();
    void _next_frame();
};

} // namespace vkl

#endif // VKLEARNIN_ENGINE_ENGINE_HPP