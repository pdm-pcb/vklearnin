#ifndef VKLEARNIN_RENDERING_TARGETS_DEPTHBUFFER_HPP
#define VKLEARNIN_RENDERING_TARGETS_DEPTHBUFFER_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"

namespace vkl {

class DepthBuffer final {
public:
    void create(vk::Rect2D const &render_area,
                vk::Format const format,
                vk::SampleCountFlagBits const samples);

    void destroy();

    inline auto const & render_area() const { return _render_area;  }
    inline auto const & view()        const { return _image.view;   }

    DepthBuffer();
    ~DepthBuffer() = default;

    DepthBuffer(DepthBuffer &&other) noexcept;
    DepthBuffer(const DepthBuffer &) = delete;

    DepthBuffer & operator=(DepthBuffer &&) = delete;
    DepthBuffer & operator=(const DepthBuffer &) = delete;

private:
    vk::Rect2D  _render_area;
    ImageObject _image;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_TARGETS_COLORBUFFER_HPP