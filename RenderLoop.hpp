#ifndef VKL_RENDERLOOP_HPP
#define VKL_RENDERLOOP_HPP

#include "common.hpp"

class Instance;
class CommandQueues;
class Swapchain;
class Pipeline;
class Framebuffers;

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
    bool run(const Instance &instance, const CommandQueues &queues,
             Swapchain &swapchain, Pipeline &pipeline,
             Framebuffers &framebuffers);

    // -------------------------------------------------------------------------
    // Setup
    void init_synchronization();

    RenderLoop(const ::VkDevice &device, Window &window, CommandQueues &queues);
    ~RenderLoop();

private:
    std::array<::VkSemaphore, MAX_IMAGES> _image_available_sems;
    std::array<::VkSemaphore, MAX_IMAGES> _draw_complete_sems;
    std::array<::VkFence, MAX_IMAGES>     _display_fences;

    const ::VkDevice &_device;
    Window           &_window;
    CommandQueues    &_queues;

    void _image_resized(const Instance &instance, Swapchain &swapchain,
                        Pipeline &pipeline, Framebuffers &framebuffers);
};

#endif // VKL_RENDERLOOP_HPP