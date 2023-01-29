#ifndef VKLEARNIN_MESHES_MESH_HPP
#define VKLEARNIN_MESHES_MESH_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/buffers/VertexBuffer.hpp"

namespace vkl {

class Mesh {
public:
    inline const auto & vertex_buffer() const { return _vertex_buffer; }
    inline auto vertex_count()          const { return _vertex_data.size(); }

    Mesh();
    ~Mesh() = default;

    Mesh(Mesh &&) = delete;
    Mesh(const Mesh &) = delete;

    Mesh& operator=(Mesh &&) = delete;
    Mesh& operator=(const Mesh &) = delete;

protected:
    void _set_vertices(const std::vector<Vertex> &vertices);
    void _shutdown_buffers();

private:
    VertexBuffer        _vertex_buffer;
    std::vector<Vertex> _vertex_data;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_XYPLANE_HPP