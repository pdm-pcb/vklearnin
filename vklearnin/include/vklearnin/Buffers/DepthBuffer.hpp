#ifndef VKLEARNIN_BUFFERS_DEPTHBUFFER_HPP
#define VKLEARNIN_BUFFERS_DEPTHBUFFER_HPP

#include <vulkan/vulkan.hpp>

#include <vector>

class Instance;
class Swapchain;

// =============================================================================
class DepthBuffer {
public:
    void init_image(const ::VkImageTiling &tiling,
                    const ::VkFormatFeatureFlags &flags);
    void init_image_view();

    inline ::VkFormat    format()     const { return _format; }
    inline ::VkImageView image_view() const { return _image_view; }

    DepthBuffer(const Instance &instance, const Swapchain &swapchain);
    ~DepthBuffer();

private:
    ::VkImage        _image_handle;
    ::VkDeviceMemory _device_memory;
    ::VkImageView    _image_view;
    ::VkFormat       _format;

    const Instance  &_instance;
    const Swapchain &_swapchain;

    void _choose_format(const ::VkImageTiling &tiling,
                        const ::VkFormatFeatureFlags &flags);
};

#endif // VKLEARNIN_BUFFERS_DEPTHBUFFER_HPP