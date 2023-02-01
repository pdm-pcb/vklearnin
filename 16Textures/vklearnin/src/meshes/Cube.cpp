#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/Cube.hpp"

#include "vklearnin/meshes/Vertex.hpp"
#include "vklearnin/meshes/Index.hpp"

namespace vkl {

// =============================================================================
void Cube::init(const float scale, const float tile) {
    Mesh::_set_vertices({
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

    Mesh::_set_indices({
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

// =============================================================================
void Cube::shutdown() {
    Mesh::_shutdown_buffers();
}

} // namespace vkl