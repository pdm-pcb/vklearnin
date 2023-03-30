#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/GeneratedMesh.hpp"

namespace vkl {

void GeneratedMesh::_set_vertices(std::vector<Vertex> const &vertices) {
    _vertex_data = vertices;
    _vertex_buffer.create(sizeof(Vertex) * _vertex_data.size());
    _vertex_buffer.populate_buffer(_vertex_data);
}

void GeneratedMesh::_set_indices(std::vector<Index> const &indices) {
    _index_data = indices;
    _index_buffer.create(sizeof(Index) * _index_data.size());
    _index_buffer.populate_buffer(_index_data);
}

void GeneratedMesh::draw_indexed(CmdBuffer const &cmd_buffer) const {
    cmd_buffer.native().bindVertexBuffers(
        0u,
        1u,
        &_vertex_buffer.buffer().handle,
        _vertex_buffer.offsets().data()
    );
    cmd_buffer.native().bindIndexBuffer(
        _index_buffer.buffer().handle,
        0u,
        INDEX_TYPE
    );
    cmd_buffer.native().drawIndexed(
        static_cast<uint32_t>(_index_data.size()),
        1u, 0u, 0u, 0u
    );
}

} // namespace vkl