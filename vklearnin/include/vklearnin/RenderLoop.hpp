#ifndef VKLEARNIN_RENDERLOOP_HPP
#define VKLEARNIN_RENDERLOOP_HPP

#include "vklearnin/pch.hpp"

#include "vklearnin/System/Events/KeyboardEvent.hpp"

class Instance;
class CommandQueues;
class Swapchain;
class Pipeline;
class Framebuffers;
struct UBOList;
class UniformBufferObject;
struct DescriptorSets;
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
    bool run(const Instance &instance,
             Swapchain &swapchain,
             UBOList &ubo_list,
             Pipeline &pipeline,
             Framebuffers &framebuffers,
             DescriptorSets &descriptor_sets,
             const std::vector<Model *> &models);

    void on_keypress(const KeyPressEvent &event);
    void on_keyrelease(const KeyReleaseEvent &event);

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

    bool _running;
    bool _up;
    bool _down;

    const vk::Device &_device;
    Window           &_window;
    CommandQueues    &_queues;

    void _image_resized(const Instance &instance, Swapchain &swapchain,
                        Pipeline &pipeline, Framebuffers &framebuffers);

    void _update_per_frame(UniformBufferObject &ubo, const Swapchain &swapchain,
                           const uint32_t frame_index);

    void _update_per_object(UniformBufferObject &ubo,
                            const std::vector<Model *> &models,
                            const float runtime,
                            const uint32_t frame_index);
};

#endif // VKLEARNIN_RENDERLOOP_HPP