#ifndef VKLEARNIN_MESH_ICOSPHERE_HPP
#define VKLEARNIN_MESH_ICOSPHERE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/mesh/Vertex.hpp"
#include "vklearnin/shaders/BufferObject.hpp"

namespace vkl {

class Icosphere final {
public:
    void create_buffers();
    void destroy_buffers();

    inline const auto & vertex_buffer() const { return _vertex_buffer; }
    inline auto & vertices() { return _vertices; }

    inline const auto & index_buffer() const { return _index_buffer; }
    inline auto & indices() { return _indices; }

    Icosphere(const float scale, const uint32_t subdivisions);
    ~Icosphere() = default;
    Icosphere() = delete;

    Icosphere(Icosphere &&other) = delete;
    Icosphere(const Icosphere &other) = delete;

    Icosphere & operator=(Icosphere &&other) = delete;
    Icosphere & operator=(const Icosphere &other) = delete;

private:
    std::vector<Vertex>   _vertices;
    std::vector<uint32_t> _indices;

    BufferObject _vertex_buffer;
    BufferObject _index_buffer;
    
    // std::unordered_map<std::pair<uint32_t, uint32_t>, uint32_t> _midpoint_cache;

    glm::vec4 _normalize(const float scale, const glm::vec3 &vector);
    void _subdivide(const uint32_t subdivisions);
    uint32_t _find_midpoint(const uint32_t index_a, const uint32_t index_b);
};

} // namespace vkl
#endif // VKLEARNIN_MESH_ICOSPHERE_HPP