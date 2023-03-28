#ifndef VKLEARNIN_MESHES_MESH_HPP
#define VKLEARNIN_MESHES_MESH_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/VertexTypes.hpp"
#include "vklearnin/resources/buffers/VertexBuffer.hpp"
#include "vklearnin/resources/buffers/IndexBuffer.hpp"
#include "vklearnin/system/devices/CmdBuffer.hpp"

namespace vkl {

template <typename VertexType>
class Mesh {
public:
    void destroy() {
        _index_buffer.destroy();
        _vertex_buffer.destroy();
    }

    void draw_indexed(CmdBuffer const &cmd_buffer) const;

    Mesh() = default;
    ~Mesh() = default;

    Mesh(Mesh &&) = delete;
    Mesh(const Mesh &) = delete;

    Mesh& operator=(Mesh &&) = delete;
    Mesh& operator=(Mesh const &) = delete;

protected:
    void _set_vertices(std::vector<VertexType> const &vertices) {
        _vertex_data = vertices;
        _vertex_buffer.create(sizeof(VertexType) * _vertex_data.size());
        _vertex_buffer.populate_buffer(_vertex_data);
    }

    void _set_indices(std::vector<Index> const &indices) {
        _index_data = indices;
        _index_buffer.create(sizeof(Index) * _index_data.size());
        _index_buffer.populate_buffer(_index_data);
    }

private:
    VertexBuffer<VertexType>    _vertex_buffer;
    std::vector<VertexType>     _vertex_data;

    IndexBuffer        _index_buffer;
    std::vector<Index> _index_data;
};

template <typename VertexType>
void Mesh<VertexType>::draw_indexed(CmdBuffer const &cmd_buffer) const {
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

#endif // VKLEARNIN_MESHES_XYPLANE_HPP