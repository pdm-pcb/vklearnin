#ifndef VKLEARNIN_RENDERINGSWAPCHAIN_SWAPCHAIN_HPP
#define VKLEARNIN_RENDERINGSWAPCHAIN_SWAPCHAIN_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"

namespace vkl {

class CmdQueue;

class Swapchain final {
public:
    static void next_image();
    static void reset_fence();

    static void submit(const std::vector<vk::CommandBuffer> &buffers);
    static void present();

    static void create();
    static void destroy();

    inline static auto image_index() { return _draw_index; }
    inline static auto& image(const uint32_t index) { return _images[index]; }

    inline static const auto& extent()       { return _extent; };
    inline static const auto& offset()       { return _offset; };
    inline static const auto& image_format() { return _image_format; }
    inline static auto
    render_area() { return vk::Rect2D { _offset, _extent }; }

    Swapchain() = delete;
    ~Swapchain() = delete;

    Swapchain(Swapchain &&) = delete;
    Swapchain(const Swapchain &) = delete;

    Swapchain& operator=(Swapchain &&) = delete;
    Swapchain& operator=(const Swapchain &) = delete;

private:
    static vk::Format         _image_format;
    static vk::ColorSpaceKHR  _color_space;
    static vk::Extent2D       _extent;
    static vk::Offset2D       _offset;
    static vk::PresentModeKHR _present_mode;
    
    static vk::SwapchainCreateInfoKHR _create_info;
    static vk::SwapchainKHR _swapchain;

    struct ImageSync {
        vk::Semaphore present_complete;
        vk::Semaphore draw_complete;
        vk::Fence     queue_fence;
    };

    static std::vector<ImageObject> _images;
    static std::vector<ImageSync> _image_sync;

    static uint32_t _draw_index;
    static uint32_t _present_index;

    static void _query_surface_capabilities();
    static void _query_surface_format();
    static void _query_surface_present_modes();
    static void _populate_create_info();
    static void _get_images();
    static void _create_image_views();
    static void _create_sync_primitives();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERINGSWAPCHAIN_SWAPCHAIN_HPP