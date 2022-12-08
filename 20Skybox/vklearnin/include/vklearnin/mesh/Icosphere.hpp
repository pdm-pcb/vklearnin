#ifndef VKLEARNIN_MESH_ICOSPHERE_HPP
#define VKLEARNIN_MESH_ICOSPHERE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/mesh/Vertex.hpp"
#include "vklearnin/mesh/Face.hpp"
#include "vklearnin/shaders/BufferObject.hpp"

// Credit where credit is due - without the following, this class would not
// exist. =)
// 
// http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
// https://schneide.blog/2016/07/15/generating-an-icosphere-in-c/
// https://mft-dev.dk/uv-mapping-sphere/

namespace vkl {

class Icosphere final {
public:
    void create_buffers();
    void destroy_buffers();

    inline const auto & vertex_buffer() const { return _vertex_buffer; }
    inline auto & vertices() { return _vertices; }

    inline const auto & index_buffer() const { return _index_buffer; }
    inline auto & faces() { return _faces; }

    Icosphere(const float scale, const uint32_t subdivisions);
    ~Icosphere() = default;
    Icosphere() = delete;

    Icosphere(Icosphere &&other) = delete;
    Icosphere(const Icosphere &other) = delete;

    Icosphere & operator=(Icosphere &&other) = delete;
    Icosphere & operator=(const Icosphere &other) = delete;

private:
    struct Edge {
        Index _a = std::numeric_limits<Index>::max();
        Index _b = std::numeric_limits<Index>::max();

        Edge(const Index a, const Index b) :
            _a { a }, _b { b }
        { }

        bool operator==(const Edge &other) const {
            return _a == other._a && _b == other._b;
        }
    };

    struct EdgeHash {
        size_t operator()(const Edge edge) const {
            size_t hash_a = std::hash<Index>()(edge._a);
            size_t hash_b = std::hash<Index>()(edge._b);
            return hash_a ^ hash_b;
        }
    };

    using MidpointCache = std::unordered_map<Edge, Index, EdgeHash>;
    
    std::vector<Vertex> _vertices;
    std::vector<Face>   _faces;

    BufferObject _vertex_buffer;
    BufferObject _index_buffer;

    glm::vec4 _normalize(const float scale, const glm::vec3 &vector);

    void _subdivide(const uint32_t subdivisions, const float scale);
    Index _find_midpoint(const Index index_a, const Index index_b,
                         const float scale, MidpointCache &cache);
                    
    
    void _generate_UVs();
};

} // namespace vkl
#endif // VKLEARNIN_MESH_ICOSPHERE_HPP