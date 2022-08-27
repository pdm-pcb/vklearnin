#ifndef VKLEARNIN_SWAPCHAIN_HPP
#define VKLEARNIN_SWAPCHAIN_HPP

#include "vklearnin/common.hpp"

#include <vulkan/vulkan.h>

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
    void init_extent(const ::VkExtent2D &extent);
    void init_swapchain(const CommandQueues &queues); // make the thing real
    void init_swapchain_images(); // grab the image/buffer data
    void init_image_views();      // create the views for the images we've got

    // -------------------------------------------------------------------------
    // Recovery
    void destroy();
    void create(const ::VkExtent2D &extent, const CommandQueues &queues,
                const ::VkSurfaceKHR &surface);

    // -------------------------------------------------------------------------
    // For those concerned with swapchain atributes

    inline const ::VkRect2D render_area() const {
        return { _offset, _extent };
    }
    inline const std::pair<uint32_t, uint32_t> extent() const {
        return { _extent.width, _extent.height };
    }
    inline const std::pair<int32_t, int32_t> offset() const {
        return { _offset.x, _offset.y };
    }
    inline ::VkFormat color_format() const {
        return _color_format;
    }
    inline const std::array<::VkImageView, MAX_IMAGES> & image_views() const {
        return _image_views;
    }
    inline const ::VkSwapchainKHR & swapchain() const {
        return _swapchain;
    }
    inline float aspect_ratio() const {
        return _aspect_ratio;
    }


    Swapchain(const Instance &instance, ::VkSurfaceKHR &surface);
    ~Swapchain();

    Swapchain() = delete;

private:
    ::VkFormat        _color_format;
    ::VkColorSpaceKHR _color_space;

    uint32_t           _image_count;
    uint32_t           _image_array_layers;
    ::VkOffset2D       _offset;
    ::VkExtent2D       _extent;
    float              _aspect_ratio;
    ::VkPresentModeKHR _present_mode;

    ::VkSurfaceTransformFlagBitsKHR _transform;

    ::VkSwapchainKHR _swapchain;
    ::VkSwapchainKHR _old_swapchain;

    std::array<::VkImage, MAX_IMAGES>     _images;
    std::array<::VkImageView, MAX_IMAGES> _image_views;

    const Instance &_instance;
    ::VkSurfaceKHR &_surface;
};

#endif // VKLEARNIN_SWAPCHAIN_HPP