#ifndef VKL_RENDERLOOP_HPP
#define VKL_RENDERLOOP_HPP

#include "Index.hpp"
#include "StagedBuffer.hpp"

#include <vulkan/vulkan.h>

class Instance;
class CommandQueues;
class Swapchain;
class Pipeline;
class Framebuffers;
class UniformBufferObject;
class DescriptorSet;

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
             const std::vector<::VkBuffer> &vertex_buffers,
             const std::vector<::VkDeviceSize> &vertex_buffer_offsets,
             const StagedBuffer<Index> &index_buffer);

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

    void _update_ubo(UniformBufferObject &ubo, const Swapchain &swapchain,
                     const uint32_t frame_index);
};

#endif // VKL_RENDERLOOP_HPP