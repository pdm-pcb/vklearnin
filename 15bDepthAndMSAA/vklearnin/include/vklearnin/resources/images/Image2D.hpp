#ifndef VKLEARNIN_RESOURCES_IMAGES_IMAGE2D_HPP
#define VKLEARNIN_RESOURCES_IMAGES_IMAGE2D_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"

namespace vkl {

class Image2D final {
public:
    void init(const vk::Image image, const vk::Format format,
              const vk::ImageLayout layout, const vk::Extent2D &extent);

    inline auto & image() { return _image; }

    Image2D();
    ~Image2D() = default;

    Image2D(Image2D &&) = delete;
    Image2D(const Image2D &) = delete;

    Image2D& operator=(Image2D &&) = delete;
    Image2D& operator=(const Image2D &) = delete;

private:
    ImageObject  _image;
    vk::Extent2D _extent;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERINGIMAGES_IMAGE2D_HPP