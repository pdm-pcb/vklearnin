#ifndef VKL_SWAPCHAIN_HPP
#define VKL_SWAPCHAIN_HPP

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>
#include <utility>

class Instance;
class CommandQueues;

#if defined(__linux__)
    class X11Window;
#elif defined(_WIN32)
    class Win32Window;
#endif

//==============================================================================
class Swapchain {
public:
    void init_color_format();  // first up, color space/image format
    void init_present_modes(); // next, choosing a presentation mode
    void init_extent();        // finally, the images' physical dimensions
    void init_swapchain(const CommandQueues &queue); // make the thing real

    void init_swapchain_images(); // grab the image/buffer data
    void init_image_views();      // create the views for the images we've got

    // -------------------------------------------------------------------------
    // For those concerned with swapchain atributes

    inline const std::pair<uint32_t, uint32_t> extent() const {
        return { _extent.width, _extent.height };
    }
    inline const std::pair<int32_t, int32_t> offset() const {
        return { _offset.x, _offset.y };
    }
    inline ::VkFormat color_format() const {
        return _color_format;
    }
    inline const std::vector<::VkImageView> & image_views() const {
        return _image_views;
    }
    inline const ::VkSwapchainKHR & swapchain() const {
        return _swapchain;
    }

#if defined(__linux__)
    Swapchain(const Instance &instance, const X11Window &window);
#elif defined (_WIN32)
    Swapchain(const Instance &instance, const Win32Window &window);
#endif
    ~Swapchain();

    Swapchain() = delete;

private:
    const Instance &_instance;

#if defined(__linux__)
    const X11Window &_window; 
#elif defined(_WIN32)
    const Win32Window &_window; 
#endif

    ::VkFormat        _color_format;
    ::VkColorSpaceKHR _color_space;

    uint32_t           _image_count;
    uint32_t           _image_array_layers;
    ::VkExtent2D       _extent;
    ::VkOffset2D       _offset;
    ::VkPresentModeKHR _present_mode;

    ::VkSurfaceTransformFlagBitsKHR _transform;

    ::VkSwapchainKHR _swapchain;

    std::vector<::VkImage>     _images;
    std::vector<::VkImageView> _image_views;
};

#endif // VKL_SWAPCHAIN_HPP