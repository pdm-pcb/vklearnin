#ifndef VKLEARNIN_MESHES_MESH_HPP
#define VKLEARNIN_MESHES_MESH_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/VertexTypes.hpp"
#include "vklearnin/resources/buffers/VertexBuffer.hpp"
#include "vklearnin/resources/buffers/IndexBuffer.hpp"

namespace vkl {

template <typename VertexType>
class Mesh {
public:
    inline auto const& vertex_buffer() const { return _vertex_buffer; }
    inline auto vertex_count()         const { return _vertex_data.size(); }

    inline auto const& index_buffer() const { return _index_buffer; }
    inline auto index_count()         const { return _index_data.size(); }

    Mesh() = default;
    ~Mesh() = default;

    Mesh(Mesh &&) = delete;
    Mesh(const Mesh &) = delete;

    Mesh& operator=(Mesh &&) = delete;
    Mesh& operator=(const Mesh &) = delete;

protected:
    void _set_vertices(const std::vector<VertexType> &vertices) {
        _vertex_data = vertices;
        _vertex_buffer.init(sizeof(VertexType) * _vertex_data.size());
        _vertex_buffer.populate_buffer(_vertex_data);
    }

    void _set_indices(const std::vector<Index> &indices) {
        _index_data = indices;
        _index_buffer.init(sizeof(Index) * _index_data.size());
        _index_buffer.populate_buffer(_index_data);
    }

    void _shutdown_buffers() {
        _index_buffer.shutdown();
        _vertex_buffer.shutdown();
    }

private:
    VertexBuffer<VertexType> _vertex_buffer;
    std::vector<VertexType>  _vertex_data;

    IndexBuffer        _index_buffer;
    std::vector<Index> _index_data;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_XYPLANE_HPP