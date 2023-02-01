#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/XYPlane.hpp"

#include "vklearnin/meshes/Vertex.hpp"
#include "vklearnin/meshes/Index.hpp"

namespace vkl {

// =============================================================================
void XYPlane::init(const float scale, const float tile) {
    Mesh::_set_vertices({
        {{ -scale, -scale,  scale, 1.0f }, { 0.0f, tile }},
        {{  scale, -scale,  scale, 1.0f }, { tile, tile }},
        {{  scale,  scale,  scale, 1.0f }, { tile, 0.0f }},
        {{ -scale,  scale,  scale, 1.0f }, { 0.0f, 0.0f }},
    });

    Mesh::_set_indices({
        0, 1, 2,
        0, 2, 3
    });
}

// =============================================================================
void XYPlane::shutdown() {
    Mesh::_shutdown_buffers();
}

} // namespace vkl