#ifndef VKLEARNIN_RENDERING_SWAPCHAIN_SWAPCHAIN_HPP
#define VKLEARNIN_RENDERING_SWAPCHAIN_SWAPCHAIN_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"

namespace vkl {

class FrameData;

class Swapchain final {
public:
    static void submit_and_present(FrameData const &frame);

    static void create();
    static void destroy();

    inline static auto const& extent()       { return _extent;       }
    inline static auto const& offset()       { return _offset;       }
    inline static auto const& render_area()  { return _render_area;  }
    inline static auto const& images()       { return _images;       }
    inline static auto        image_format() { return _image_format; }

    Swapchain() = delete;

private:
    static vk::Extent2D       _extent;
    static vk::Offset2D       _offset;
    static vk::Rect2D         _render_area;
    static vk::Format         _image_format;
    static vk::ColorSpaceKHR  _color_space;
    static vk::PresentModeKHR _present_mode;

    static vk::SwapchainKHR _swapchain;

    static std::vector<ImageObject> _images;

    static void _query_surface_capabilities();
    static void _query_surface_format();
    static void _query_surface_present_modes();
    static void _populate_create_info(vk::SwapchainCreateInfoKHR &create_info);
    static void _get_images();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_SWAPCHAIN_SWAPCHAIN_HPP