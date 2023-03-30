#ifndef VKLEARNIN_MESHES_GENERATEDMESH_HPP
#define VKLEARNIN_MESHES_GENERATEDMESH_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/buffers/VertexBuffer.hpp"
#include "vklearnin/resources/buffers/IndexBuffer.hpp"
#include "vklearnin/system/devices/CmdBuffer.hpp"

namespace vkl {

class GeneratedMesh {
public:
    void destroy() {
        _index_buffer.destroy();
        _vertex_buffer.destroy();
    }

    void draw_indexed(CmdBuffer const &cmd_buffer) const;

    GeneratedMesh() = default;
    ~GeneratedMesh() = default;

    GeneratedMesh(GeneratedMesh &&) = delete;
    GeneratedMesh(const GeneratedMesh &) = delete;

    GeneratedMesh& operator=(GeneratedMesh &&) = delete;
    GeneratedMesh& operator=(GeneratedMesh const &) = delete;

protected:
    void _set_vertices(std::vector<Vertex> const &vertices);
    void _set_indices(std::vector<Index> const &indices);

private:
    VertexBuffer        _vertex_buffer;
    std::vector<Vertex> _vertex_data;

    IndexBuffer        _index_buffer;
    std::vector<Index> _index_data;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_GENERATEDMESH_HPP