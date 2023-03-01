#ifndef VKLEARNIN_MESHES_SKYBOX_HPP
#define VKLEARNIN_MESHES_SKYBOX_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/Mesh.hpp"

namespace vkl {

template <typename VertexType>
class Skybox final : public Mesh<VertexType> {
public:
    using CornerColors = std::array<std::array<float, 4>, 8>;

    void init(float const scale, CornerColors const corner_colors)
    requires std::is_same_v<VertexType, VertexFlatColor>
    {
        Mesh<VertexType>::_set_vertices({
            {{ -scale, -scale,  scale, 1.0f }, corner_colors[0]},
            {{ -scale,  scale,  scale, 1.0f }, corner_colors[1]},
            {{  scale,  scale,  scale, 1.0f }, corner_colors[2]},
            {{  scale, -scale,  scale, 1.0f }, corner_colors[3]},

            {{  scale, -scale, -scale, 1.0f }, corner_colors[4]},
            {{  scale,  scale, -scale, 1.0f }, corner_colors[5]},
            {{ -scale,  scale, -scale, 1.0f }, corner_colors[6]},
            {{ -scale, -scale, -scale, 1.0f }, corner_colors[7]},
        });

        Mesh<VertexType>::_set_indices({
            // front face
            0, 2, 1,
            0, 3, 2,

            // back face
            4, 6, 5,
            4, 7, 6,

            // top face
            1, 5, 6,
            1, 2, 5,

            // bottom face
            7, 3, 0,
            7, 4, 3,

            // left face
            7, 1, 6,
            7, 0, 1,

            // right face
            3, 5, 2,
            3, 4, 5,
        });
    }

    void init(float const scale, float const tile)
    requires std::is_same_v<VertexType, VertexFlatTexture>
    {
        Mesh<VertexType>::_set_vertices({
            // front face
            {{ -scale, -scale,  scale, 1.0f }, { 0.0f, 0.0f }}, // 0
            {{ -scale,  scale,  scale, 1.0f }, { 0.0f, tile }}, // 1
            {{  scale,  scale,  scale, 1.0f }, { tile, tile }}, // 2
            {{  scale, -scale,  scale, 1.0f }, { tile, 0.0f }}, // 3

            // back face
            {{  scale, -scale, -scale, 1.0f }, { tile, 0.0f }}, // 4
            {{  scale,  scale, -scale, 1.0f }, { tile, tile }}, // 5
            {{ -scale,  scale, -scale, 1.0f }, { 0.0f, tile }}, // 6
            {{ -scale, -scale, -scale, 1.0f }, { 0.0f, 0.0f }}, // 7

            // top face
            {{ -scale,  scale,  scale, 1.0f }, { 0.0f, 0.0f }}, // 8
            {{ -scale,  scale, -scale, 1.0f }, { 0.0f, tile }}, // 9
            {{  scale,  scale, -scale, 1.0f }, { tile, tile }}, // 10
            {{  scale,  scale,  scale, 1.0f }, { tile, 0.0f }}, // 11

            // bottom face
            {{ -scale, -scale, -scale, 1.0f }, { tile, 0.0f }}, // 12
            {{ -scale, -scale,  scale, 1.0f }, { tile, tile }}, // 13
            {{  scale, -scale,  scale, 1.0f }, { 0.0f, tile }}, // 14
            {{  scale, -scale, -scale, 1.0f }, { 0.0f, 0.0f }}, // 15

            // left face
            {{ -scale, -scale, -scale, 1.0f }, { 0.0f, 0.0f }}, // 16
            {{ -scale,  scale, -scale, 1.0f }, { 0.0f, tile }}, // 17
            {{ -scale,  scale,  scale, 1.0f }, { tile, tile }}, // 18
            {{ -scale, -scale,  scale, 1.0f }, { tile, 0.0f }}, // 19

            // right face
            {{  scale, -scale,  scale, 1.0f }, { tile, 0.0f }}, // 20
            {{  scale,  scale,  scale, 1.0f }, { tile, tile }}, // 21
            {{  scale,  scale, -scale, 1.0f }, { 0.0f, tile }}, // 22
            {{  scale, -scale, -scale, 1.0f }, { 0.0f, 0.0f }}, // 23
        });

        Mesh<VertexType>::_set_indices({
            // front face
            0, 2, 1,
            0, 3, 2,

            // back face
            4, 6, 5,
            4, 7, 6,

            // top face
            8, 10,  9,
            8, 11, 10,

            // bottom face
            12, 14, 13,
            12, 15, 14,

            // left face
            16, 18, 17,
            16, 19, 18,

            // right face
            20, 22, 21,
            20, 23, 22,
        });
    }

    void shutdown() { Mesh<VertexType>::_shutdown_buffers(); }

    Skybox() = default;
    ~Skybox() = default;

    Skybox(Skybox &&) = delete;
    Skybox(Skybox const&) = delete;

    Skybox& operator=(Skybox &&) = delete;
    Skybox& operator=(Skybox const&) = delete;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_SKYBOX_HPP