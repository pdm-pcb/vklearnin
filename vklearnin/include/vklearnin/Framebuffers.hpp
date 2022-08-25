#ifndef VKLEARNIN_FRAMEBUFFERS_HPP
#define VKLEARNIN_FRAMEBUFFERS_HPP

#include <vulkan/vulkan.hpp>

#include <vector>

class Instance;
class Swapchain;
class Pipeline;

// =============================================================================
class Framebuffers {
public:
    void init_buffers(const Swapchain &swapchain, const Pipeline &pipeline);

    void destroy();
    void create(const Swapchain &swapchain, const Pipeline &pipeline);

    inline const ::VkFramebuffer & buffer(const size_t index) const {
        return _buffers[index];
    }

    Framebuffers(const ::VkDevice &device);
    ~Framebuffers();

private:
    std::vector<::VkFramebuffer> _buffers;

    const ::VkDevice &_device;
};

#endif // VKLEARNIN_FRAMEBUFFERS_HPP