#ifndef VKLEARNIN_RENDERING_SWAPCHAIN_HPP
#define VKLEARNIN_RENDERING_SWAPCHAIN_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class PhysicalDevice;
class LogicalDevice;

class Swapchain final {
public:
    void create();
    void destroy();

    Swapchain(const PhysicalDevice &physical_device,
              LogicalDevice  &logical_device,
              const vk::SurfaceKHR &surface);
    ~Swapchain() = default;

    Swapchain() = delete;

    Swapchain(Swapchain &&other) = delete;
    Swapchain(const Swapchain &other) = delete;

    Swapchain & operator=(Swapchain &&other) = delete;
    Swapchain & operator=(const Swapchain &other) = delete;

private:
    vk::SwapchainCreateInfoKHR _create_info;

    vk::Format        _surface_format;
    vk::ColorSpaceKHR _color_space;

    std::vector<vk::Image> _images;
    std::vector<vk::ImageView> _image_views;
    uint32_t _current_image_index;

    vk::Offset2D _offset;
    vk::Extent2D _extent;

    vk::PresentModeKHR _present_mode;
    vk::SwapchainKHR   _swapchain;

    const PhysicalDevice &_physical_device;
    LogicalDevice        &_logical_device;
    const vk::SurfaceKHR &_surface;

    void _query_surface_capabilities();
    void _query_surface_format();
    void _query_surface_present_modes();
    void _set_create_info();
    void _get_images();
    void _create_image_views();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_SWAPCHAIN_HPP