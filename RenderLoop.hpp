#ifndef VKL_RENDERLOOP_HPP
#define VKL_RENDERLOOP_HPP

#include <vulkan/vulkan.h>

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
    bool run(const Instance &instance, const Swapchain &swapchain,
             const Pipeline &pipeline, const Framebuffers &framebuffers);

    // -------------------------------------------------------------------------
    // Setup
    void init_synchronization();

    RenderLoop(const ::VkDevice &device, Window &window, CommandQueues &queues);
    ~RenderLoop();

private:
    ::VkSemaphore _image_available_sem;
    ::VkSemaphore _draw_complete_sem;
    ::VkFence     _display_fence;

    const ::VkDevice &_device;
    Window           &_window;
    CommandQueues    &_queues;
};

#endif // VKL_RENDERLOOP_HPP