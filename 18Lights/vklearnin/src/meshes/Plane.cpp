#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/Plane.hpp"

namespace vkl {

void Plane::init(float const scale, Vec4 const &color, float const tile) {
    GeneratedMesh::_set_vertices({
        {{ -scale, -scale, 0.0f, 1.0f }, color, Vec4::unit_z, { 0.0f, tile }},
        {{ -scale,  scale, 0.0f, 1.0f }, color, Vec4::unit_z, { tile, tile }},
        {{  scale,  scale, 0.0f, 1.0f }, color, Vec4::unit_z, { tile, 0.0f }},
        {{  scale, -scale, 0.0f, 1.0f }, color, Vec4::unit_z, { 0.0f, 0.0f }},

    });

    GeneratedMesh::_set_indices({
        0, 1, 2,
        0, 2, 3
    });
}

} // namespace vkl