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
        {{  scale, -scale, 0.0f, 1.0f }, corner_colors[3] },
    };

    // The vertex buffer will match the size of the vertex data exactly.
    _vertex_buffer.init(sizeof(Vertex) * _vertex_data.size());

    // Now the buffer is ready to receive the data we specified above
    _vertex_buffer.populate_buffer(_vertex_data);

    _index_data = {
        {{ 0, 1, 2 }},
        {{ 0, 2, 3 }}
    };
    
    _index_buffer.init(sizeof(Face) * _index_data.size());
    _index_buffer.populate_buffer(_index_data);
}

// =============================================================================
void XYPlane::shutdown() {
    _index_buffer.shutdown();
    _vertex_buffer.shutdown();
}

// =============================================================================
XYPlane::XYPlane() :
    _vertex_buffer { },
    _vertex_data   { },
    _index_buffer  { },
    _index_data    { }
{ }

} // namespace vkl