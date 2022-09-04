#ifndef VKLEARNIN_BUFFERS_DEPTHBUFFER_HPP
#define VKLEARNIN_BUFFERS_DEPTHBUFFER_HPP

#include "vklearnin/Tools/Allocator.hpp"

#include "vklearnin/pch.hpp"

#include <vector>

class Instance;
class Swapchain;

// =============================================================================
class DepthBuffer {
public:
    void init_image(const vk::ImageTiling &tiling,
                    const vk::FormatFeatureFlags &flags);
    void init_image_view();

    inline vk::Format    format()     const { return _format; }
    inline vk::ImageView image_view() const { return _image_view; }

    DepthBuffer(const Instance &instance, const Swapchain &swapchain);
    ~DepthBuffer();

    DepthBuffer(DepthBuffer &&other) = delete;
    DepthBuffer(const DepthBuffer &other) = delete;

    DepthBuffer & operator=(DepthBuffer &&other) = delete;
    DepthBuffer & operator=(const DepthBuffer &other) = delete;

private:
    vk::Image     _image_handle;
    VmaAllocation _device_memory;
    vk::ImageView _image_view;
    vk::Format    _format;

    const Instance  &_instance;
    const Swapchain &_swapchain;

    void _choose_format(const vk::ImageTiling &tiling,
                        const vk::FormatFeatureFlags &flags);
};

#endif // VKLEARNIN_BUFFERS_DEPTHBUFFER_HPP