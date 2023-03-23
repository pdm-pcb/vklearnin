#ifndef VKLEARNIN_MESHES_CUBE_HPP
#define VKLEARNIN_MESHES_CUBE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/Mesh.hpp"

namespace vkl {

template <typename VertexType>
class Cube final : public Mesh<VertexType> {
public:
    using CornerColors = std::array<Vec4, 8>;

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

        Mesh<VertexType>::_set_indices(_shared_indices);
    }

    void init(float const scale, Vec4 const &corner_color)
    requires std::is_same_v<VertexType, VertexFlatColor>
    {
        Mesh<VertexType>::_set_vertices({
            {{ -scale, -scale,  scale, 1.0f }, corner_color},
            {{ -scale,  scale,  scale, 1.0f }, corner_color},
            {{  scale,  scale,  scale, 1.0f }, corner_color},
            {{  scale, -scale,  scale, 1.0f }, corner_color},

            {{  scale, -scale, -scale, 1.0f }, corner_color},
            {{  scale,  scale, -scale, 1.0f }, corner_color},
            {{ -scale,  scale, -scale, 1.0f }, corner_color},
            {{ -scale, -scale, -scale, 1.0f }, corner_color},
        });

        Mesh<VertexType>::_set_indices(_shared_indices);
    }

    void init(float const scale, float const tile)
    requires std::is_same_v<VertexType, VertexTexture>
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

        Mesh<VertexType>::_set_indices(_separate_indices);
    }

    void init(float const scale, Vec4 const &color)
    requires std::is_same_v<VertexType, VertexLitColor>
    {
        Mesh<VertexType>::_set_vertices({
            // front face
            {{ -scale, -scale,  scale, 1.0f },  Vec4::unit_z, color }, // 0
            {{ -scale,  scale,  scale, 1.0f },  Vec4::unit_z, color }, // 1
            {{  scale,  scale,  scale, 1.0f },  Vec4::unit_z, color }, // 2
            {{  scale, -scale,  scale, 1.0f },  Vec4::unit_z, color }, // 3

            // back face
            {{  scale, -scale, -scale, 1.0f }, -Vec4::unit_z, color }, // 4
            {{  scale,  scale, -scale, 1.0f }, -Vec4::unit_z, color }, // 5
            {{ -scale,  scale, -scale, 1.0f }, -Vec4::unit_z, color }, // 6
            {{ -scale, -scale, -scale, 1.0f }, -Vec4::unit_z, color }, // 7

            // top face
            {{ -scale,  scale,  scale, 1.0f },  Vec4::unit_y, color }, // 8
            {{ -scale,  scale, -scale, 1.0f },  Vec4::unit_y, color }, // 9
            {{  scale,  scale, -scale, 1.0f },  Vec4::unit_y, color }, // 10
            {{  scale,  scale,  scale, 1.0f },  Vec4::unit_y, color }, // 11

            // bottom face
            {{ -scale, -scale, -scale, 1.0f }, -Vec4::unit_y, color }, // 12
            {{ -scale, -scale,  scale, 1.0f }, -Vec4::unit_y, color }, // 13
            {{  scale, -scale,  scale, 1.0f }, -Vec4::unit_y, color }, // 14
            {{  scale, -scale, -scale, 1.0f }, -Vec4::unit_y, color }, // 15

            // left face
            {{ -scale, -scale, -scale, 1.0f }, -Vec4::unit_x, color }, // 16
            {{ -scale,  scale, -scale, 1.0f }, -Vec4::unit_x, color }, // 17
            {{ -scale,  scale,  scale, 1.0f }, -Vec4::unit_x, color }, // 18
            {{ -scale, -scale,  scale, 1.0f }, -Vec4::unit_x, color }, // 19

            // right face
            {{  scale, -scale,  scale, 1.0f },  Vec4::unit_x, color }, // 20
            {{  scale,  scale,  scale, 1.0f },  Vec4::unit_x, color }, // 21
            {{  scale,  scale, -scale, 1.0f },  Vec4::unit_x, color }, // 22
            {{  scale, -scale, -scale, 1.0f },  Vec4::unit_x, color }, // 23
        });

        Mesh<VertexType>::_set_indices(_separate_indices);
    }

    void init(float const scale, float const tile)
    requires std::is_same_v<VertexType, VertexMaterial>
    {
        Mesh<VertexType>::_set_vertices({
            // front face
            {{ -scale, -scale,  scale, 1.0f },  Vec4::unit_z, { 0.0f, 0.0f }}, // 0
            {{ -scale,  scale,  scale, 1.0f },  Vec4::unit_z, { 0.0f, tile }}, // 1
            {{  scale,  scale,  scale, 1.0f },  Vec4::unit_z, { tile, tile }}, // 2
            {{  scale, -scale,  scale, 1.0f },  Vec4::unit_z, { tile, 0.0f }}, // 3

            // back face
            {{  scale, -scale, -scale, 1.0f }, -Vec4::unit_z, { tile, 0.0f }}, // 4
            {{  scale,  scale, -scale, 1.0f }, -Vec4::unit_z, { tile, tile }}, // 5
            {{ -scale,  scale, -scale, 1.0f }, -Vec4::unit_z, { 0.0f, tile }}, // 6
            {{ -scale, -scale, -scale, 1.0f }, -Vec4::unit_z, { 0.0f, 0.0f }}, // 7

            // top face
            {{ -scale,  scale,  scale, 1.0f },  Vec4::unit_y, { 0.0f, 0.0f }}, // 8
            {{ -scale,  scale, -scale, 1.0f },  Vec4::unit_y, { 0.0f, tile }}, // 9
            {{  scale,  scale, -scale, 1.0f },  Vec4::unit_y, { tile, tile }}, // 10
            {{  scale,  scale,  scale, 1.0f },  Vec4::unit_y, { tile, 0.0f }}, // 11

            // bottom face
            {{ -scale, -scale, -scale, 1.0f }, -Vec4::unit_y, { tile, 0.0f }}, // 12
            {{ -scale, -scale,  scale, 1.0f }, -Vec4::unit_y, { tile, tile }}, // 13
            {{  scale, -scale,  scale, 1.0f }, -Vec4::unit_y, { 0.0f, tile }}, // 14
            {{  scale, -scale, -scale, 1.0f }, -Vec4::unit_y, { 0.0f, 0.0f }}, // 15

            // left face
            {{ -scale, -scale, -scale, 1.0f }, -Vec4::unit_x, { 0.0f, 0.0f }}, // 16
            {{ -scale,  scale, -scale, 1.0f }, -Vec4::unit_x, { 0.0f, tile }}, // 17
            {{ -scale,  scale,  scale, 1.0f }, -Vec4::unit_x, { tile, tile }}, // 18
            {{ -scale, -scale,  scale, 1.0f }, -Vec4::unit_x, { tile, 0.0f }}, // 19

            // right face
            {{  scale, -scale,  scale, 1.0f },  Vec4::unit_x, { tile, 0.0f }}, // 20
            {{  scale,  scale,  scale, 1.0f },  Vec4::unit_x, { tile, tile }}, // 21
            {{  scale,  scale, -scale, 1.0f },  Vec4::unit_x, { 0.0f, tile }}, // 22
            {{  scale, -scale, -scale, 1.0f },  Vec4::unit_x, { 0.0f, 0.0f }}, // 23
        });

        Mesh<VertexType>::_set_indices(_separate_indices);
    }

    Cube() = default;
    ~Cube() = default;

    Cube(Cube &&) = delete;
    Cube(Cube const&) = delete;

    Cube& operator=(Cube &&) = delete;
    Cube& operator=(Cube const&) = delete;

private:
    std::vector<Index> const _shared_indices {
        // front face
        0, 1, 2,
        0, 2, 3,

        // back face
        4, 5, 6,
        4, 6, 7,

        // top face
        1, 6, 5,
        1, 5, 2,

        // bottom face
        7, 0, 3,
        7, 3, 4,

        // left face
        7, 6, 1,
        7, 1, 0,

        // right face
        3, 2, 5,
        3, 5, 4,
    };

    std::vector<Index> const _separate_indices {
        // front face
        0, 1, 2,
        0, 2, 3,

        // back face
        4, 5, 6,
        4, 6, 7,

        // top face
        8,  9, 10,
        8, 10, 11,

        // bottom face
        12, 13, 14,
        12, 14, 15,

        // left face
        16, 17, 18,
        16, 18, 19,

        // right face
        20, 21, 22,
        20, 22, 23,
    };
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_CUBE_HPP