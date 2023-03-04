#ifndef VKLEARNIN_MESHES_XYPLANE_HPP
#define VKLEARNIN_MESHES_XYPLANE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/Mesh.hpp"

namespace vkl {

template <typename VertexType>
class XYPlane final : public Mesh<VertexType> {
public:
    using CornerColors = std::array<std::array<float, 4>, 4>;

    void init(float const scale, CornerColors const corner_colors)
    requires std::is_same_v<VertexType, VertexFlatColor>
    {
        Mesh<VertexType>::_set_vertices({
            {{ -scale, -scale, 0.0f, 1.0f }, corner_colors[0]},
            {{ -scale,  scale, 0.0f, 1.0f }, corner_colors[1]},
            {{  scale,  scale, 0.0f, 1.0f }, corner_colors[2]},
            {{  scale, -scale, 0.0f, 1.0f }, corner_colors[3]},
        });

        Mesh<VertexType>::_set_indices({
            0, 1, 2,
            0, 2, 3
        });
    }

    void init(float const scale, float const tile)
    requires std::is_same_v<VertexType, VertexFlatTexture>
    {
        Mesh<VertexType>::_set_vertices({
            {{ -scale, -scale, 0.0f, 1.0f }, { 0.0f, tile }},
            {{ -scale,  scale, 0.0f, 1.0f }, { tile, tile }},
            {{  scale,  scale, 0.0f, 1.0f }, { tile, 0.0f }},
            {{  scale, -scale, 0.0f, 1.0f }, { 0.0f, 0.0f }},
        });

        Mesh<VertexType>::_set_indices({
            0, 1, 2,
            0, 2, 3
        });
    }

    XYPlane() = default;
    ~XYPlane() = default;

    XYPlane(XYPlane &&) = delete;
    XYPlane(XYPlane const&) = delete;

    XYPlane& operator=(XYPlane &&) = delete;
    XYPlane& operator=(XYPlane const&) = delete;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_XYPLANE_HPP