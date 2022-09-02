#ifndef VKLEARNIN_RENDERLOOP_HPP
#define VKLEARNIN_RENDERLOOP_HPP

#include <vulkan/vulkan.hpp>

class Instance;
class CommandQueues;
class Swapchain;
class Pipeline;
class Framebuffers;
class UniformBufferObject;
class DescriptorSet;
class DepthBuffer;
class Model;

#include <vector>

#if defined(__linux__)
    class X11Window;
#elif defined(_WIN32)
    class Win32Window;
#endif

// =============================================================================
class RenderLoop {

#if defined(__linux__)
    using Window = X11Window;
#elif defined(_WIN32)
    using Window = Win32Window;
#endif

public:
    // main render loop: window events, then draw
    bool run(const Instance &instance, Swapchain &swapchain,
             UniformBufferObject &ubo, Pipeline &pipeline,
             DescriptorSet &descriptor_set, Framebuffers &framebuffers,
             const std::vector<Model *> &models);

    // -------------------------------------------------------------------------
    // Setup
    void init_synchronization();

    RenderLoop(const vk::Device &device, Window &window, CommandQueues &queues);
    ~RenderLoop();

    RenderLoop() = delete;

    RenderLoop(RenderLoop &&other) = delete;
    RenderLoop(const RenderLoop &other) = delete;

    RenderLoop & operator=(RenderLoop &&other) = delete;
    RenderLoop & operator=(const RenderLoop &other) = delete;

private:
    std::array<vk::Semaphore, FRAME_OVERLAP> _image_available_sems;
    std::array<vk::Semaphore, FRAME_OVERLAP> _draw_complete_sems;
    std::array<vk::Fence, FRAME_OVERLAP>     _display_fences;

    const vk::Device &_device;
    Window           &_window;
    CommandQueues    &_queues;

    void _image_resized(const Instance &instance, Swapchain &swapchain,
                        Pipeline &pipeline, Framebuffers &framebuffers);

    void _update_ubo(UniformBufferObject &ubo, const Swapchain &swapchain,
                     const uint32_t frame_index, const float runtime);
};

#endif // VKLEARNIN_RENDERLOOP_HPP