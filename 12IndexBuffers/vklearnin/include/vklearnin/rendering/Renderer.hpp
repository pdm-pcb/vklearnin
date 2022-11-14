#ifndef VKLEARNIN_RENDERING_RENDERER_HPP
#define VKLEARNIN_RENDERING_RENDERER_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/Framebuffer.hpp"

namespace vkl {

class GraphicsInstance;
class LogicalDevice;
class Swapchain;
class Pipeline;

struct BufferObject;

class Renderer final {
public:
    void draw(const BufferObject &vertex_buffer,
              const BufferObject &index_buffer,
              const uint32_t index_count);
    void present();

    void init();
    void shutdown();

    Renderer(GraphicsInstance &graphics_instance, LogicalDevice &logical_device,
             Swapchain &swapchain, Pipeline &pipeline);
    ~Renderer() = default;

    Renderer() = delete;

    Renderer(Renderer &&) = delete;
    Renderer(const Renderer &) = delete;

    Renderer & operator=(Renderer &&) = delete;
    Renderer & operator=(const Renderer &) = delete;

private:
    std::vector<vk::Semaphore> _image_available_sems;
    std::vector<vk::Semaphore> _draw_complete_sems;
    std::vector<vk::Fence>     _present_fences;

    uint32_t _current_framebuffer;
    uint32_t _target_image_index;

    std::vector<Framebuffer> _framebuffers;

    GraphicsInstance &_graphics_instance;
    LogicalDevice    &_logical_device;
    Swapchain        &_swapchain;
    Pipeline         &_pipeline;

    void _next_swapchain_image_index();
    void _image_resized();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_RENDERER_HPP