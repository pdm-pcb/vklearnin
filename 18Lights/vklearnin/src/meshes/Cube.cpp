#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/Cube.hpp"

namespace vkl {

void Cube::init(float const scale, Vec4 const &color, float const tile) {
    GeneratedMesh::_set_vertices({
        // front face
        {{ -scale, -scale,  scale, 1.0f }, color, Vec4::unit_z, { 0.0f, 0.0f }},
        {{ -scale,  scale,  scale, 1.0f }, color, Vec4::unit_z, { 0.0f, tile }},
        {{  scale,  scale,  scale, 1.0f }, color, Vec4::unit_z, { tile, tile }},
        {{  scale, -scale,  scale, 1.0f }, color, Vec4::unit_z, { tile, 0.0f }},

        // back face
        {{  scale, -scale, -scale, 1.0f }, color, -Vec4::unit_z, { tile, 0.0f }},
        {{  scale,  scale, -scale, 1.0f }, color, -Vec4::unit_z, { tile, tile }},
        {{ -scale,  scale, -scale, 1.0f }, color, -Vec4::unit_z, { 0.0f, tile }},
        {{ -scale, -scale, -scale, 1.0f }, color, -Vec4::unit_z, { 0.0f, 0.0f }},

        // top face
        {{ -scale,  scale,  scale, 1.0f }, color, Vec4::unit_y, { 0.0f, 0.0f }},
        {{ -scale,  scale, -scale, 1.0f }, color, Vec4::unit_y, { 0.0f, tile }},
        {{  scale,  scale, -scale, 1.0f }, color, Vec4::unit_y, { tile, tile }},
        {{  scale,  scale,  scale, 1.0f }, color, Vec4::unit_y, { tile, 0.0f }},

        // bottom face
        {{ -scale, -scale, -scale, 1.0f }, color, -Vec4::unit_y, { tile, 0.0f }},
        {{ -scale, -scale,  scale, 1.0f }, color, -Vec4::unit_y, { tile, tile }},
        {{  scale, -scale,  scale, 1.0f }, color, -Vec4::unit_y, { 0.0f, tile }},
        {{  scale, -scale, -scale, 1.0f }, color, -Vec4::unit_y, { 0.0f, 0.0f }},

        // left face
        {{ -scale, -scale, -scale, 1.0f }, color, -Vec4::unit_x, { 0.0f, 0.0f }},
        {{ -scale,  scale, -scale, 1.0f }, color, -Vec4::unit_x, { 0.0f, tile }},
        {{ -scale,  scale,  scale, 1.0f }, color, -Vec4::unit_x, { tile, tile }},
        {{ -scale, -scale,  scale, 1.0f }, color, -Vec4::unit_x, { tile, 0.0f }},

        // right face
        {{  scale, -scale,  scale, 1.0f }, color, Vec4::unit_x, { tile, 0.0f }},
        {{  scale,  scale,  scale, 1.0f }, color, Vec4::unit_x, { tile, tile }},
        {{  scale,  scale, -scale, 1.0f }, color, Vec4::unit_x, { 0.0f, tile }},
        {{  scale, -scale, -scale, 1.0f }, color, Vec4::unit_x, { 0.0f, 0.0f }},
    });

    GeneratedMesh::_set_indices({
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
    });
}

} // namespace vkl
