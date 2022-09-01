#ifndef VKLEARNIN_BUFFERS_FRAMEBUFFERS_HPP
#define VKLEARNIN_BUFFERS_FRAMEBUFFERS_HPP

#include <vulkan/vulkan.hpp>

#include <vector>

class Instance;
class Swapchain;
class Pipeline;
class DepthBuffer;

// =============================================================================
class Framebuffers {
public:
    void init_buffers(const Swapchain &swapchain, const Pipeline &pipeline);

    // -------------------------------------------------------------------------
    // In the event of recreating the swapchain
    void destroy();
    void create(const Swapchain &swapchain, const Pipeline &pipeline);

    inline const vk::Framebuffer & buffer(const size_t index) const {
        return _buffers[index];
    }

    Framebuffers(const vk::Device &device);
    ~Framebuffers();

private:
    std::vector<vk::Framebuffer> _buffers;

    const vk::Device &_device;
};

#endif // VKLEARNIN_BUFFERS_FRAMEBUFFERS_HPP