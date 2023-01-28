#ifndef VKLEARNIN_RENDER_SWAPCHAIN_SWAPCHAIN_HPP
#define VKLEARNIN_RENDER_SWAPCHAIN_SWAPCHAIN_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Image2D;
class CmdQueue;

class Swapchain final {
public:
    static void create();
    static void destroy();

    inline static auto image_index() { return _draw_index; }
    inline static auto & image(const uint32_t index) { return _images[index]; }

    Swapchain() = delete;
    ~Swapchain() = delete;

    Swapchain(Swapchain &&) = delete;
    Swapchain(const Swapchain &) = delete;

    Swapchain & operator=(Swapchain &&) = delete;
    Swapchain & operator=(const Swapchain &) = delete;

private:
    static vk::Format         _image_format;
    static vk::ColorSpaceKHR  _color_space;
    static vk::Extent2D       _extent;
    static vk::Offset2D       _offset;
    static vk::PresentModeKHR _present_mode;
    
    static vk::SwapchainCreateInfoKHR _create_info;
    static vk::SwapchainKHR _swapchain;

    struct ImageSync {
        vk::Semaphore available;
        vk::Semaphore draw_complete;
        vk::Fence     present_fence;
    };

    static std::vector<Image2D *> _images;
    static std::vector<ImageSync> _image_sync;

    static uint32_t _draw_index;
    static uint32_t _present_index;

    static void _query_surface_capabilities();
    static void _query_surface_format();
    static void _query_surface_present_modes();
    static void _populate_create_info();
    static void _get_images();
    static void _create_image_views();
    static void _create_sync();
};

} // namespace vkl

#endif // VKLEARNIN_RENDER_SWAPCHAIN_SWAPCHAIN_HPP