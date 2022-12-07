#include "vklearnin/vklearnin.hpp"
#include "vklearnin/mesh/Icosphere.hpp"

namespace vkl {

// =============================================================================
void Icosphere::create_buffers() {
    auto vertex_buffer_size = _vertices.size() * sizeof(Vertex);
    auto index_buffer_size = _indices.size() * sizeof(uint32_t);

    CONSOLE_TRACE(
        "Allocating Cube buffers: {} vb {} ib",
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
void Icosphere::_subdivide(const uint32_t subdivisions) {
    std::vector<Vertex>   new_vertices;
    std::vector<uint32_t> new_indices;

    new_vertices.reserve(_vertices.size() * 4);
    new_indices.reserve(_indices.size() * 4);

    for(uint32_t sub = 0; sub < subdivisions; ++sub) {
        for(uint32_t index = 0u; index < _indices.size(); index += 3) {
            auto a = _indices[index + 0u];
            auto b = _indices[index + 1u];
            auto c = _indices[index + 2u];

            auto ab = _find_midpoint(a, b);
            auto bc = _find_midpoint(b, c);
            auto ca = _find_midpoint(c, a);

            new_indices.push_back(a);
            new_indices.push_back(ab);
            new_indices.push_back(ca);
            new_vertices.push_back({{ a, ab, ca, 1.0f }, {0.0f, 0.0f }});

            new_indices.push_back(b);
            new_indices.push_back(bc);
            new_indices.push_back(ab);
            new_vertices.push_back({{ b, bc, ab, 1.0f }, {0.0f, 1.0f }});

            new_indices.push_back(c);
            new_indices.push_back(ca);
            new_indices.push_back(bc);
            new_vertices.push_back({{ c, ca, bc, 1.0f }, {1.0f, 0.0f }});

            new_indices.push_back(ab);
            new_indices.push_back(bc);
            new_indices.push_back(ca);
            new_vertices.push_back({{ ab, bc, ca, 1.0f }, {1.0f, 1.0f }});
        }
    }
}

// =============================================================================
uint32_t Icosphere::_find_midpoint(const uint32_t index_a,
                                   const uint32_t index_b)
{
    return 0u;
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

    _subdivide(subdivisions);
}

} // namespace vkl