#ifndef VKLEARNIN_MESHES_XZPLANE_HPP
#define VKLEARNIN_MESHES_XZPLANE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/Mesh.hpp"

namespace vkl {

template <typename VertexType>
class XZPlane final : public Mesh<VertexType> {
public:
    using CornerColors = std::array<std::array<float, 4>, 4>;

    void init(float const scale, CornerColors const corner_colors)
    requires std::is_same_v<VertexType, VertexFlatColor>
    {
        Mesh<VertexType>::_set_vertices({
            {{ -scale, 0.0f,  scale, 1.0f }, corner_colors[0]},
            {{ -scale, 0.0f, -scale, 1.0f }, corner_colors[1]},
            {{  scale, 0.0f, -scale, 1.0f }, corner_colors[2]},
            {{  scale, 0.0f,  scale, 1.0f }, corner_colors[3]},
        });

        Mesh<VertexType>::_set_indices({
            0, 1, 2,
            0, 2, 3
        });
    }

    void init(float const scale, float const tile)
    requires std::is_same_v<VertexType, VertexTexture>
    {
        Mesh<VertexType>::_set_vertices({
            {{ -scale, 0.0f,  scale, 1.0f }, { 0.0f, tile }},
            {{ -scale, 0.0f, -scale, 1.0f }, { tile, tile }},
            {{  scale, 0.0f, -scale, 1.0f }, { tile, 0.0f }},
            {{  scale, 0.0f,  scale, 1.0f }, { 0.0f, 0.0f }},
        });

        Mesh<VertexType>::_set_indices({
            0, 1, 2,
            0, 2, 3
        });
    }

    void init(float const scale, float const tile)
    requires std::is_same_v<VertexType, VertexMaterial>
    {
        Mesh<VertexType>::_set_vertices({
            {{ -scale, 0.0f,  scale, 1.0f }, Vec4::unit_y, { 0.0f, tile }},
            {{ -scale, 0.0f, -scale, 1.0f }, Vec4::unit_y, { tile, tile }},
            {{  scale, 0.0f, -scale, 1.0f }, Vec4::unit_y, { tile, 0.0f }},
            {{  scale, 0.0f,  scale, 1.0f }, Vec4::unit_y, { 0.0f, 0.0f }},
        });

        Mesh<VertexType>::_set_indices({
            0, 1, 2,
            0, 2, 3
        });
    }

    XZPlane() = default;
    ~XZPlane() = default;

    XZPlane(XZPlane &&) = delete;
    XZPlane(XZPlane const&) = delete;

    XZPlane& operator=(XZPlane &&) = delete;
    XZPlane& operator=(XZPlane const&) = delete;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_XZPLANE_HPP