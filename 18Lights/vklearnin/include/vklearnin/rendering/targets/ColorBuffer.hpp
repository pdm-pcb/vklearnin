#ifndef VKLEARNIN_RENDERING_TARGETS_COLORBUFFER_HPP
#define VKLEARNIN_RENDERING_TARGETS_COLORBUFFER_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"

namespace vkl {

class ColorBuffer final {
public:
    void create(vk::Rect2D const &render_area,
                vk::Format const format,
                vk::SampleCountFlagBits const samples);

    void destroy();

    inline auto const & render_area() const { return _render_area;  }
    inline auto const & view()        const { return _image.view;   }

    ColorBuffer();
    ~ColorBuffer() = default;

    ColorBuffer(ColorBuffer &&other) noexcept;
    ColorBuffer(const ColorBuffer &) = delete;

    ColorBuffer & operator=(ColorBuffer &&) = delete;
    ColorBuffer & operator=(const ColorBuffer &) = delete;

private:
    vk::Rect2D  _render_area;
    ImageObject _image;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_TARGETS_COLORBUFFER_HPP