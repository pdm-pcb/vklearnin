#ifndef VKLEARNIN_MESH_CUBE_HPP
#define VKLEARNIN_MESH_CUBE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/mesh/Vertex.hpp"
#include "vklearnin/mesh/Face.hpp"
#include "vklearnin/shaders/BufferObject.hpp"

namespace vkl {

class LogicalDevice;

class Cube final {
public:
    void create_buffers();
    void destroy_buffers();

    inline const auto & vertex_buffer() const { return _vertex_buffer; }
    inline auto & vertices() { return _vertices; }

    inline const auto & index_buffer() const { return _index_buffer; }
    inline auto & faces() { return _faces; }

    Cube(const float scale, const float tile);
    ~Cube() = default;
    Cube() = delete;

    Cube(Cube &&) = delete;
    Cube(const Cube &) = delete;

    Cube & operator=(Cube &&) = delete;
    Cube & operator=(const Cube &) = delete;

private:
    std::vector<Vertex> _vertices;
    std::vector<Face>   _faces;

    BufferObject _vertex_buffer;
    BufferObject _index_buffer;
};

} // namespace vkl

#endif // VKLEARNIN_MESH_CUBE_HPP