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
    struct Edge {
        uint32_t index_a = static_cast<uint32_t>(-1);
        uint32_t index_b = static_cast<uint32_t>(-1);

        Edge(const uint32_t a, const uint32_t b) :
            index_a { a },
            index_b { b }
        { }

        bool operator==(const Edge &other) const {
            return index_a == other.index_a &&
                   index_b == other.index_b;
        }
    };

    struct EdgeHash {
        size_t operator()(const Edge edge) const {
            size_t hash_a = std::hash<uint32_t>()(edge.index_a);
            size_t hash_b = std::hash<uint32_t>()(edge.index_b);

            return hash_a ^ hash_b;
        }
    };
    
    std::vector<Vertex>   _vertices;
    std::vector<uint32_t> _indices;

    BufferObject _vertex_buffer;
    BufferObject _index_buffer;

    std::unordered_map<Edge, uint32_t, EdgeHash> _midpoint_cache;

    glm::vec4 _normalize(const float scale, const glm::vec3 &vector);
    void _subdivide(const uint32_t subdivisions, const float scale);
    uint32_t _midpoint(const uint32_t index_a, const uint32_t index_b,
                       const uint32_t next_index, glm::vec4 &midpoint);
};

} // namespace vkl
#endif // VKLEARNIN_MESH_ICOSPHERE_HPP