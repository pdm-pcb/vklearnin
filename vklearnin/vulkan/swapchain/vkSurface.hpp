#ifndef VKLEARNIN_VULKAN_SWAPCHAIN_VKSURFACE_HPP
#define VKLEARNIN_VULKAN_SWAPCHAIN_VKSURFACE_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

class TargetWindow;
class vkInstance;
class vkPhysicalDevice;

class vkSurface final {
public:
    vkSurface() = default;
    ~vkSurface() = default;

    vkSurface(vkSurface &&) = delete;
    vkSurface(const vkSurface &) = delete;

    vkSurface & operator=(vkSurface &&) = delete;
    vkSurface & operator=(const vkSurface &) = delete;

    struct Config final {
        bool enable_vsync = false;
    };

    [[nodiscard]] bool create(TargetWindow const &target_window,
                              vkInstance const &instance,
                              Config const &config);
    bool destroy();

    [[nodiscard]] bool check_details(vkPhysicalDevice const &device);

    [[nodiscard]] inline auto const & native()  const { return _handle; }
    [[nodiscard]] inline auto min_image_count() const { return _min_image_count; }
    [[nodiscard]] inline auto max_image_count() const { return _max_image_count; }
    [[nodiscard]] inline auto const & extent()  const { return _extent; }
    [[nodiscard]] inline auto const & format()  const { return _format; }
    [[nodiscard]] inline auto present_mode()    const { return _present_mode; }
    [[nodiscard]] inline auto aspect_ratio()    const { return _aspect_ratio; }

private:
    vk::SurfaceKHR _handle   { nullptr };
    vk::Instance   _instance { nullptr };

    bool _enable_vsync { false };

    vk::Extent2D _extent          { };
    float        _aspect_ratio    { 0.0f };
    uint32_t     _min_image_count { 0u };
    uint32_t     _max_image_count { 0u };

    vk::SurfaceFormatKHR _format { };
    vk::PresentModeKHR   _present_mode { vk::PresentModeKHR::eImmediate };

    [[nodiscard]] bool _check_capabilities(vk::PhysicalDevice const &device);
    [[nodiscard]] bool _check_formats(vk::PhysicalDevice const &device);
    [[nodiscard]] bool _check_present_modes(vk::PhysicalDevice const &device);

    void _reset();
};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_SWAPCHAIN_VKSURFACE_HPP