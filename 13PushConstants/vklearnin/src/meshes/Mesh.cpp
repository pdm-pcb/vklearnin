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
void Mesh::_set_indices(const std::vector<Index> &indices) {
    _index_data = indices;
    _index_buffer.init(sizeof(indices) * _index_data.size());
    _index_buffer.populate_buffer(_index_data);
}

// =============================================================================
void Mesh::_shutdown_buffers() {
    _index_buffer.shutdown();
    _vertex_buffer.shutdown();
}

// =============================================================================
Mesh::Mesh() :
    _vertex_buffer { },
    _vertex_data   { },
    _index_buffer  { },
    _index_data    { }
{ }

} // namespace vkl