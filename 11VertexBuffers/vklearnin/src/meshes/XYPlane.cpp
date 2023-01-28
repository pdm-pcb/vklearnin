#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/XYPlane.hpp"

namespace vkl {

// =============================================================================
void XYPlane::init(const float scale, const CornerColors corner_colors) {
    // If you were to sketch these values out on a traditional Cartesian plane,
    // you'd get two overlapping triangles. Graphics hardware works first and
    // last in triangles, so a plane is defined by six points, rather than four.
    
    // Using the scale value here allows us to alter the size of the plane on
    // screen from the client code in Demo. I've chosen to insit on uniform
    // scaling for all points, so the resulting geometry will always be a plane.

    // Finally, the CornerColors only has four values so that the repeated
    // vertices between our two triangles match in appearance as well as
    // location.
    _vertex_data = {
        {{ -scale, -scale, 0.0f, 1.0f }, corner_colors[0] },
        {{ -scale,  scale, 0.0f, 1.0f }, corner_colors[1] },
        {{  scale,  scale, 0.0f, 1.0f }, corner_colors[2] },

        {{ -scale, -scale, 0.0f, 1.0f }, corner_colors[0] },
        {{  scale,  scale, 0.0f, 1.0f }, corner_colors[2] },
        {{  scale, -scale, 0.0f, 1.0f }, corner_colors[3] },
    };

    _vertex_buffer.create(
        // The vertex buffer will match the size of the vertex data exactly.
        sizeof(Vertex) * _vertex_data.size(),

        // We only want this memory to be used as a vertex buffer.
        vk::BufferUsageFlagBits::eVertexBuffer,

        // Exclusive mode means this buffer will not be shared between threads.
        vk::SharingMode::eExclusive,

        // Host visible and coherent describes memory the CPU is able to write
        // to in a predictable way.
        vk::MemoryPropertyFlagBits::eHostVisible |
        vk::MemoryPropertyFlagBits::eHostCoherent
    );

    // Now the buffer is ready to receive the data we specified above
    _vertex_buffer.populate_buffer(_vertex_data);
}

// =============================================================================
void XYPlane::shutdown() {
    _vertex_buffer.destroy();
}

// =============================================================================
XYPlane::XYPlane() :
    _vertex_buffer { }
{ }

} // namespace vkl