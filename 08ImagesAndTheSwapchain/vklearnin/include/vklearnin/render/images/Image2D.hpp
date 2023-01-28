#ifndef VKLEARNIN_RENDER_IMAGES_IMAGE2D_HPP
#define VKLEARNIN_RENDER_IMAGES_IMAGE2D_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Image2D final {
public:
    void init(const vk::Image image, const vk::Format format,
              const vk::ImageLayout layout, const vk::Extent2D &extents);
    void create_view(const vk::ImageAspectFlags &aspect_flags);
    void destroy_view();

    inline auto view() const { return _view; }

    Image2D();
    ~Image2D() = default;

    Image2D(Image2D &&) = delete;
    Image2D(const Image2D &) = delete;

    Image2D & operator=(Image2D &&) = delete;
    Image2D & operator=(const Image2D &) = delete;

private:
    vk::Image       _image;
    vk::ImageView   _view;
    vk::Format      _format;
    vk::ImageLayout _layout;
    vk::Extent2D    _extent;
};

} // namespace vkl

#endif // VKLEARNIN_RENDER_IMAGES_IMAGE2D_HPP