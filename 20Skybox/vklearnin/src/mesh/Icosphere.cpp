#include "vklearnin/vklearnin.hpp"
#include "vklearnin/mesh/Icosphere.hpp"

namespace vkl {

// =============================================================================
void Icosphere::create_buffers() {
    auto vertex_buffer_size = _vertices.size() * sizeof(Vertex);
    auto index_buffer_size = _indices.size() * sizeof(uint32_t);

    CONSOLE_TRACE(
        "Allocating Icosphere buffers: {} vb {} ib",
        vertex_buffer_size, index_buffer_size
    );

    _vertex_buffer = BufferTools::create_buffer(
        vertex_buffer_size,
        (vk::BufferUsageFlagBits::eTransferDst |
         vk::BufferUsageFlagBits::eVertexBuffer),
        vk::SharingMode::eExclusive,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        "sphere vertex"
    );

    _index_buffer = BufferTools::create_buffer(
        index_buffer_size,
        (vk::BufferUsageFlagBits::eTransferDst |
         vk::BufferUsageFlagBits::eIndexBuffer),
        vk::SharingMode::eExclusive,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        "sphere index"
    );

    BufferTools::move_to_device(_vertices.data(), _vertex_buffer);
    BufferTools::move_to_device(_indices.data(), _index_buffer);
}

// =============================================================================
void Icosphere::destroy_buffers() {
    BufferTools::destroy_buffer(_vertex_buffer);
    BufferTools::destroy_buffer(_index_buffer);
}

// =============================================================================
glm::vec4 Icosphere::_normalize(const float scale, const glm::vec3 &vector) {
    auto scaled = scale * glm::normalize(vector);
    return glm::vec4(scaled.x, scaled.y, scaled.z, 1.0f);
}

// =============================================================================
void Icosphere::_subdivide(const uint32_t subdivisions, const float scale) {
    for(uint32_t sub = 0; sub < subdivisions; ++sub) {
        std::vector<Vertex>   new_vertices;
        std::vector<uint32_t> new_indices;

        new_vertices.reserve(_vertices.size() * 4);
        new_indices.reserve(_indices.size() * 4);

        for(uint32_t index = 0u; index < _indices.size(); index += 3) {
            // original indices to inspect
            auto index_a = _indices[index + 0u];
            auto index_b = _indices[index + 1u];
            auto index_c = _indices[index + 2u];

            // store the vertex corresponding with the original index here
            new_vertices.push_back(_vertices[index_a]);
            auto new_index_a = static_cast<uint32_t>(new_vertices.size() - 1);

            // potential new vertex for the midpoint
            glm::vec4 new_vertex { 0.0f };

            // sort out the midpoint for edge ab
            // return value is the index for the vertex that bisects edge ab,
            // and "new_vertex" will have been overwritten with the above if
            // it wasn't a duplicate we already had data for
            auto mid_ab = _midpoint(
                index_a,
                index_b,
                static_cast<uint32_t>(new_vertices.size()),
                new_vertex
            );

            // in this case, new_vertex does actually have some new data in it
            // which we must track
            if(new_vertex != glm::vec4 { 0.0f }) {
                auto scaled_vert = glm::vec4(
                    scale * glm::normalize(glm::vec3(new_vertex)),
                    1.0f
                );
                new_vertices.push_back({ scaled_vert, {0.0f, 0.0f }});
                new_vertex = glm::vec4 { 0.0f };
            }

            // regardless of whether or not edge ab had a new vertex as its
            // midpoint, push the original b vertex onto the list of new verts
            // so the result of looking for edge bc's midpoint makes sense
            new_vertices.push_back(_vertices[index_b]);
            auto new_index_b = static_cast<uint32_t>(new_vertices.size() - 1);

            auto mid_bc = _midpoint(
                index_b,
                index_c,
                static_cast<uint32_t>(new_vertices.size()),
                new_vertex
            );
            if(new_vertex != glm::vec4 { 0.0f }) {
                auto scaled_vert = glm::vec4(
                    scale * glm::normalize(glm::vec3(new_vertex)),
                    1.0f
                );
                new_vertices.push_back({ scaled_vert, {0.0f, 1.0f }});
                new_vertex = glm::vec4 { 0.0f };
            }

            // likewise with edge ca - I feel like there should be a function
            // that wraps these calls to _midpoint() because they're repeatitive
            new_vertices.push_back(_vertices[index_c]);
            auto new_index_c = static_cast<uint32_t>(new_vertices.size() - 1);

            auto mid_ca = _midpoint(
                index_c,
                index_a,
                static_cast<uint32_t>(new_vertices.size()),
                new_vertex
            );
            if(new_vertex != glm::vec4 { 0.0f }) {
                auto scaled_vert = glm::vec4(
                    scale * glm::normalize(glm::vec3(new_vertex)),
                    1.0f
                );
                new_vertices.push_back({ scaled_vert, {1.0f, 0.0f }});
            }

            // now that we've got all the verts that comprise the new triangles,
            // store the indices in the correct order
            new_indices.push_back(new_index_a);
            new_indices.push_back(mid_ab);
            new_indices.push_back(mid_ca);

            new_indices.push_back(new_index_b);
            new_indices.push_back(mid_bc);
            new_indices.push_back(mid_ab);

            new_indices.push_back(new_index_c);
            new_indices.push_back(mid_ca);
            new_indices.push_back(mid_bc);

            new_indices.push_back(mid_ab);
            new_indices.push_back(mid_bc);
            new_indices.push_back(mid_ca);
        }

        CONSOLE_WARN(
            "subdivided {} vertices, {} indices into {} vertices, {} indices",
            _vertices.size(), _indices.size(),
            new_vertices.size(), new_indices.size()
        );

        std::stringstream out;
        for(uint32_t vert = 0u; vert < _vertices.size() - 2; vert += 3u) {
            out << std::format(
                "a: {:.02f}, {:.02f}, {:.02f}\t\t"
                "b: {:.02f}, {:.02f}, {:.02f}\t\t"
                "c: {:.02f}, {:.02f}, {:.02f}\n",
                _vertices[vert + 0].position.x,
                _vertices[vert + 0].position.y,
                _vertices[vert + 0].position.z,
                _vertices[vert + 1].position.x,
                _vertices[vert + 1].position.y,
                _vertices[vert + 1].position.z,
                _vertices[vert + 2].position.x,
                _vertices[vert + 2].position.y,
                _vertices[vert + 2].position.z
            );
        }
        for(uint32_t index = 0u; index < _indices.size() - 2; index += 3u) {
            out << std::format(
                "face {:04d} - "
                "{}, {}, {}\n",
                (index + 3) / 3,
                _indices[index + 0],
                _indices[index + 1],
                _indices[index + 2]
            );
        }
        CONSOLE_WARN("\n{}", out.str());

            _vertices = new_vertices;
            _indices  = new_indices;
        }

    CONSOLE_WARN(
        "Final Count: {} vertices, {} indices",
        _vertices.size(), _indices.size()
    );

    std::stringstream out;
    for(uint32_t vert = 0u; vert < _vertices.size() - 2; vert += 3u) {
        out << std::format(
            "a: {:.02f}, {:.02f}, {:.02f}\t\t"
            "b: {:.02f}, {:.02f}, {:.02f}\t\t"
            "c: {:.02f}, {:.02f}, {:.02f}\n",
            _vertices[vert + 0].position.x,
            _vertices[vert + 0].position.y,
            _vertices[vert + 0].position.z,
            _vertices[vert + 1].position.x,
            _vertices[vert + 1].position.y,
            _vertices[vert + 1].position.z,
            _vertices[vert + 2].position.x,
            _vertices[vert + 2].position.y,
            _vertices[vert + 2].position.z
        );
    }
    for(uint32_t index = 0u; index < _indices.size() - 2; index += 3u) {
        out << std::format(
            "face {:04d} - "
            "{}, {}, {}\n",
            (index + 3) / 3,
            _indices[index + 0],
            _indices[index + 1],
            _indices[index + 2]
        );
    }
    CONSOLE_WARN("\n{}", out.str());
}

// =============================================================================
uint32_t Icosphere::_midpoint(const uint32_t index_a, const uint32_t index_b,
                              const uint32_t next_index, glm::vec4 &midpoint)
{
    auto lesser_index  = std::min(index_a, index_b);
    auto greater_index = std::max(index_a, index_b);

    midpoint = glm::vec4 { 0.0f };

    auto midpoint_iter = _midpoint_cache.find({lesser_index, greater_index});
    if(midpoint_iter !=  _midpoint_cache.end()) {
        return midpoint_iter->second;
    }

    midpoint = glm::vec4(
        glm::mix(
            glm::vec3(_vertices[lesser_index].position),
            glm::vec3(_vertices[greater_index].position),
            0.5f
        ),
    1.0f);

    _midpoint_cache[{lesser_index, greater_index}] = next_index;

    return next_index;
}

// =============================================================================
Icosphere::Icosphere(const float scale, const uint32_t subdivisions) :
    _vertex_buffer { },
    _index_buffer  { }
{
    auto t = (1.0f + std::sqrt(5.0f)) * 0.5f;
    _vertices = {
        { _normalize(scale, { -1.0f,  t, 0.0f }), { 0.0f, 0.0f }},
        { _normalize(scale, {  1.0f,  t, 0.0f }), { 0.0f, 1.0f }},
        { _normalize(scale, { -1.0f, -t, 0.0f }), { 1.0f, 0.0f }},

        { _normalize(scale, {  1.0f, -t, 0.0f }), { 1.0f, 1.0f }},
        { _normalize(scale, { 0.0f, -1.0f,  t }), { 0.0f, 0.0f }},
        { _normalize(scale, { 0.0f,  1.0f,  t }), { 0.0f, 1.0f }},

        { _normalize(scale, { 0.0f, -1.0f, -t }), { 1.0f, 0.0f }},
        { _normalize(scale, { 0.0f,  1.0f, -t }), { 1.0f, 1.0f }},
        { _normalize(scale, {  t, 0.0f, -1.0f }), { 0.0f, 0.0f }},

        { _normalize(scale, {  t, 0.0f,  1.0f }), { 0.0f, 1.0f }},
        { _normalize(scale, { -t, 0.0f, -1.0f }), { 1.0f, 0.0f }},
        { _normalize(scale, { -t, 0.0f,  1.0f }), { 1.0f, 1.0f }},
    };

    _indices = {
        0, 11, 5,    0,  5,  1,     0,  1,  7,     0, 7, 10,    0, 10, 11,
        1,  5, 9,    5, 11,  4,    11, 10,  2,    10, 7,  6,    7,  1,  8,
        3,  9, 4,    3,  4,  2,     3,  2,  6,     3, 6,  8,    3,  8,  9,
        4,  9, 5,    2,  4, 11,     6,  2, 10,     8, 6,  7,    9,  8,  1,
    };

    _subdivide(subdivisions, scale);
}

} // namespace vkl