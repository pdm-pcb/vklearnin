#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/Mesh.hpp"

namespace vkl {

// =============================================================================
void Mesh::_set_vertices(const std::vector<Vertex> &vertices) {
    _vertex_data = vertices;
    _vertex_buffer.init(sizeof(Vertex) * _vertex_data.size());
    _vertex_buffer.populate_buffer(_vertex_data);
}

// =============================================================================
void Mesh::_shutdown_buffers() {
    _vertex_buffer.shutdown();
}

// =============================================================================
Mesh::Mesh() :
    _vertex_buffer { },
    _vertex_data   { }
{ }

} // namespace vkl