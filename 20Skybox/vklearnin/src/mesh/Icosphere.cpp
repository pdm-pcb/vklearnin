#include "vklearnin/vklearnin.hpp"
#include "vklearnin/mesh/Icosphere.hpp"

namespace vkl {

// =============================================================================
void Icosphere::create_buffers() {
    auto vertex_buffer_size = _vertices.size() * sizeof(Vertex);
    auto index_buffer_size  = _faces.size() * sizeof(Face);

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
    BufferTools::move_to_device(_faces.data(), _index_buffer);
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
    for(uint32_t subdivision = 0; subdivision < subdivisions; ++subdivision) {
        CONSOLE_TRACE(
            "Before subdivision {}, icosphere has {} verts and {} faces",
            subdivision + 1u,
            _vertices.size(),
            _faces.size()
        );

        // Size up the existing vertex array, as it'll necessarily grow
        _vertices.reserve(_vertices.size() * 4);

        // Create a holding place for the new indices, as their relative order
        // will change versus the existing list
        std::vector<Face> new_faces;
        new_faces.reserve(_faces.size() * 4);

        // To coordinate the 
        MidpointCache midpoint_cache;

        // Now, replace each individual triangle with four more
        for(const auto &face : _faces) {
            auto mid_ab = _midpoint(face.a, face.b, scale, midpoint_cache);
            auto mid_bc = _midpoint(face.b, face.c, scale, midpoint_cache);
            auto mid_ca = _midpoint(face.c, face.a, scale, midpoint_cache);

            new_faces.push_back({ face.a, mid_ab, mid_ca });
            new_faces.push_back({ face.b, mid_bc, mid_ab });
            new_faces.push_back({ face.c, mid_ca, mid_bc });
            new_faces.push_back({ mid_ab, mid_bc, mid_ca });
        }

        // Update the index array
        _faces = new_faces;
    }

    CONSOLE_TRACE(
        "After subdivision {}, icosphere has {} verts and {} faces",
        subdivisions,
        _vertices.size(),
        _faces.size()
    );
}

// =============================================================================
Index Icosphere::_midpoint(const Index index_a, const Index index_b,
                           const float scale, MidpointCache &cache)
{
    // Keep the keys consistent
    auto lesser_index  = std::min(index_a, index_b);
    auto greater_index = std::max(index_a, index_b);

    // Test-fit the potentially new midpoint
    auto key = MidpointCache::key_type({ lesser_index, greater_index });
    auto insert_result = cache.insert({ key, _vertices.size() });

    // Did we create a new entry in the map?
    if(insert_result.second) {
        // Then the new vertex needs a value
        auto new_vertex = glm::mix(
            glm::vec3(_vertices[index_a].position),
            glm::vec3(_vertices[index_b].position),
            0.5f
        );

        // Keep the new vertex on the sphere itself
        auto scaled_vertex = _normalize(scale, new_vertex);

        // And add to the global list
        _vertices.push_back({ scaled_vertex, { 0.0f, 0.0f } });
    }

    // Send the corresponding index value back
    return insert_result.first->second;
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

    _faces = {
        { 0,  11,  5 }, { 0,   5,  1 }, { 0,   1,  7 }, { 0,   7, 10 },
        { 0,  10, 11 }, { 1,   5,  9 }, { 5,  11,  4 }, { 11, 10,  2 },
        { 10,  7,  6 }, { 7,   1,  8 }, { 3,   9,  4 }, { 3,   4,  2 },
        { 3,   2,  6 }, { 3,   6,  8 }, { 3,   8,  9 }, { 4,   9,  5 },
        { 2,   4, 11 }, { 6,   2, 10 }, { 8,   6,  7 }, { 9,   8,  1 },
    };

    _subdivide(subdivisions, scale);
}

} // namespace vkl