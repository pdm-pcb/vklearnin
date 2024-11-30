#ifndef VKLEARNIN_VULKAN_SWAPCHAIN_VKSWAPCHAIN_HPP
#define VKLEARNIN_VULKAN_SWAPCHAIN_VKSWAPCHAIN_HPP

#include "vklearnin/pch.hpp"

#include "vklearnin/vulkan/resources/vkImage.hpp"
#include "vklearnin/vulkan/resources/vkImageView.hpp"

namespace vkl {

class vkDevice;
class vkSurface;
class vkQueue;
class vkFrameSync;

class vkSwapchain final {
public:
    vkSwapchain() = default;
    ~vkSwapchain() = default;

    vkSwapchain(vkSwapchain &&) = delete;
    vkSwapchain(vkSwapchain const &) = delete;

    vkSwapchain & operator=(vkSwapchain &&) = delete;
    vkSwapchain & operator=(vkSwapchain const &) = delete;

    bool create(vkDevice const &device, vkSurface const &surface,
                uint32_t min_image_offset = 0u);
    bool destroy();

    uint32_t acquire_next_image(vk::Semaphore const &signal_sem) const;

    inline auto const & native() const { return _handle; }
    inline auto image_count() const { return _image_count; }

    inline std::span<vkImage const> const images() const {
        return _images;
    }

    inline std::span<vkImageView const> const image_views() const {
        return _image_views;
    }

private:
    vk::SwapchainKHR _handle  { nullptr };
    vkSurface const *_surface { nullptr };
    vkDevice  const *_device  { nullptr };

    uint32_t _image_count { 0u };

    std::vector<vkImage>       _images { };
    std::vector<vkImageView>   _image_views { };

    vk::SwapchainCreateInfoKHR _create_info { };

    void _populate_create_info();
    void _get_images();
};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_SWAPCHAIN_VKSWAPCHAIN_HPP