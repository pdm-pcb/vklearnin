#ifndef VKLEARNIN_VULKAN_RESOURCES_VKIMAGEVIEW_HPP
#define VKLEARNIN_VULKAN_RESOURCES_VKIMAGEVIEW_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

class vkDevice;

class vkImageView final {
public:
    vkImageView() = default;
    ~vkImageView() = default;

    vkImageView(vkImageView &&other);
    vkImageView(vkImageView const &) = delete;

    vkImageView & operator=(vkImageView &&) = delete;
    vkImageView & operator=(vkImageView const &) = delete;

    struct Details final {
        vk::Image image { nullptr };
        vk::Format format { };
        vk::ImageViewType type { };
        vk::ImageAspectFlags aspect_flags { };
    };

    bool create(Details const &details, vkDevice const &device);

    bool destroy();

    inline auto const & native() const { return _handle; }

private:
    vk::ImageView _handle { nullptr };
    vk::Device    _device { nullptr };
};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_RESOURCES_VKIMAGEVIEW_HPP