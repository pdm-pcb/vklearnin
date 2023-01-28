#ifndef VKLEARNIN_MESHES_XYPLANE_HPP
#define VKLEARNIN_MESHES_XYPLANE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/buffers/VertexBuffer.hpp"

namespace vkl {

class XYPlane {
public:
    using CornerColors = std::array<std::array<float, 4>, 4>;

    void init(const float scale, const CornerColors corner_colors);
    void shutdown();

    inline const auto & vertex_buffer() const { return _vertex_buffer; }
    inline auto vertex_count() const { return _vertex_data.size(); }

    XYPlane();
    ~XYPlane() = default;

    XYPlane(XYPlane &&) = delete;
    XYPlane(const XYPlane &) = delete;

    XYPlane & operator=(XYPlane &&) = delete;
    XYPlane & operator=(const XYPlane &) = delete;

private:
    std::vector<Vertex> _vertex_data;
    VertexBuffer        _vertex_buffer;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_XYPLANE_HPP