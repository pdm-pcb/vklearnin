#ifndef VKLEARNIN_SWAPCHAIN_HPP
#define VKLEARNIN_SWAPCHAIN_HPP

#include "vklearnin/common.hpp"

#include <vulkan/vulkan.hpp>

class Instance;
class CommandQueues;

//==============================================================================
class Swapchain {
public:
    // -------------------------------------------------------------------------
    // Setup

    void init_color_format();  // first up, color space/image format
    void init_present_modes(); // next, choosing a presentation mode
    // finally, the images' physical dimensions
    void init_extent(const vk::Extent2D &extent);
    void init_swapchain(const CommandQueues &queues); // make the thing real
    void init_swapchain_images(); // grab the image/buffer data
    void init_image_views();      // create the views for the images we've got

    // -------------------------------------------------------------------------
    // Recovery
    void destroy();
    void create(const vk::Extent2D &extent, const CommandQueues &queues,
                const vk::SurfaceKHR &surface);

    // -------------------------------------------------------------------------
    // For those concerned with swapchain atributes

    inline const vk::Rect2D render_area() const {
        return { _offset, _extent };
    }
    inline const std::pair<uint32_t, uint32_t> extent() const {
        return { _extent.width, _extent.height };
    }
    inline const std::pair<int32_t, int32_t> offset() const {
        return { _offset.x, _offset.y };
    }
    inline vk::Format color_format() const {
        return _color_format;
    }
    inline const std::vector<vk::ImageView> & image_views() const {
        return _image_views;
    }
    inline const vk::SwapchainKHR & swapchain() const {
        return _swapchain;
    }
    inline float aspect_ratio() const {
        return _aspect_ratio;
    }


    Swapchain(const Instance &instance, vk::SurfaceKHR &surface);
    ~Swapchain();

    Swapchain() = delete;

    Swapchain(Swapchain &&other) = delete;
    Swapchain(const Swapchain &other) = delete;

    Swapchain & operator=(Swapchain &&other) = delete;
    Swapchain & operator=(const Swapchain &other) = delete;

private:
    vk::Format        _color_format;
    vk::ColorSpaceKHR _color_space;

    uint32_t           _image_count;
    uint32_t           _image_array_layers;
    vk::Offset2D       _offset;
    vk::Extent2D       _extent;
    float              _aspect_ratio;
    vk::PresentModeKHR _present_mode;

    vk::SurfaceTransformFlagBitsKHR _transform;

    vk::SwapchainKHR _swapchain;
    vk::SwapchainKHR _old_swapchain;

    std::vector<vk::Image>     _images;
    std::vector<vk::ImageView> _image_views;

    const Instance &_instance;
    vk::SurfaceKHR &_surface;
};

#endif // VKLEARNIN_SWAPCHAIN_HPP