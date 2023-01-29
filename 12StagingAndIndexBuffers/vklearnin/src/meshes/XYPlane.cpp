#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/XYPlane.hpp"

#include "vklearnin/meshes/Vertex.hpp"
#include "vklearnin/meshes/Index.hpp"

namespace vkl {

// =============================================================================
void XYPlane::init(const float scale, const CornerColors corner_colors) {
    Mesh::_set_vertices({
        {{ -scale, -scale, 0.0f, 1.0f }, corner_colors[0]},
        {{ -scale,  scale, 0.0f, 1.0f }, corner_colors[1]},
        {{  scale,  scale, 0.0f, 1.0f }, corner_colors[2]},
        {{  scale, -scale, 0.0f, 1.0f }, corner_colors[3]},
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