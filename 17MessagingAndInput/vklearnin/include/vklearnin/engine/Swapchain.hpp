#ifndef VKLEARNIN_ENGINE_SWAPCHAIN_HPP
#define VKLEARNIN_ENGINE_SWAPCHAIN_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class CmdQueue;

class Swapchain final {
public:
    vk::Result next_image(const uint32_t frame_index);
    void reset_fence() const;

    void submit(const vk::CommandBuffer &command_buffer,
                const CmdQueue &cmd_queue) const;
    vk::Result present() const;

    void create();
    void destroy();

    inline auto image_index() const { return _image_index; }

    using Offset = std::pair<int16_t, int16_t>;
    inline const auto offset() const { return Offset(_offset.x, _offset.y); }

    inline const auto extent() const { return _extent; }
    inline const auto & image_view(const uint32_t index) const {
        return _image_views[index];
    }
    inline const vk::Rect2D render_area() const { return { _offset, _extent }; }

    inline const auto & surface_format() const { return _surface_format; }
    inline const auto & native()         const { return _swapchain;      }
    inline const auto & image_views()    const { return _image_views; }

    Swapchain();
    ~Swapchain() = default;

    Swapchain(Swapchain &&other) = delete;
    Swapchain(const Swapchain &other) = delete;

    Swapchain & operator=(Swapchain &&other) = delete;
    Swapchain & operator=(const Swapchain &other) = delete;

private:
    vk::SwapchainCreateInfoKHR _create_info;

    vk::Format        _surface_format;
    vk::ColorSpaceKHR _color_space;

    std::vector<vk::Image>     _images;
    std::vector<vk::ImageView> _image_views;

    uint32_t _image_index;

    vk::Offset2D _offset;
    vk::Extent2D _extent;

    vk::PresentModeKHR _present_mode;
    vk::SwapchainKHR   _swapchain;

    std::vector<vk::Semaphore> _image_available_sems;
    std::vector<vk::Semaphore> _draw_complete_sems;
    std::vector<vk::Fence>     _present_fences;

    void _query_surface_capabilities();
    void _query_surface_format();
    void _query_surface_present_modes();
    void _set_create_info();
    void _set_extent(const vk::Extent2D &extent);
    void _get_images();
    void _create_image_views();
    void _create_synchronization();
};

} // namespace vkl

#endif // VKLEARNIN_ENGINE_SWAPCHAIN_HPP