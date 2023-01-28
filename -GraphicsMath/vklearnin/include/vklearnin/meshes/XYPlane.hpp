#ifndef VKLEARNIN_MESHES_XYPLANE_HPP
#define VKLEARNIN_MESHES_XYPLANE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/buffers/VertexBuffer.hpp"
#include "vklearnin/resources/buffers/IndexBuffer.hpp"

namespace vkl {

class XYPlane {
public:
    using CornerColors = std::array<std::array<float, 4>, 4>;

    void init(const float scale, const CornerColors corner_colors);
    void shutdown();

    inline const auto & vertex_buffer() const { return _vertex_buffer; }
    inline auto vertex_count()          const { return _vertex_data.size(); }

    inline const auto & index_buffer() const { return _index_buffer; }
    inline auto index_count()const {
        return _index_data.size() * INDICES_PER_FACE;
    }

    XYPlane();
    ~XYPlane() = default;

    XYPlane(XYPlane &&) = delete;
    XYPlane(const XYPlane &) = delete;

    XYPlane & operator=(XYPlane &&) = delete;
    XYPlane & operator=(const XYPlane &) = delete;

private:
    VertexBuffer        _vertex_buffer;
    std::vector<Vertex> _vertex_data;
    IndexBuffer         _index_buffer;
    std::vector<Face>   _index_data;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_XYPLANE_HPP