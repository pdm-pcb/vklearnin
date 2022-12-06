#include "vklearnin/vklearnin.hpp"
#include "vklearnin/mesh/Cube.hpp"

namespace vkl {

void Cube::create_buffers() {
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
        "cube vertex"
    );

    _index_buffer = BufferTools::create_buffer(
        index_buffer_size,
        (vk::BufferUsageFlagBits::eTransferDst |
         vk::BufferUsageFlagBits::eIndexBuffer),
        vk::SharingMode::eExclusive,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        "cube index"
    );

    BufferTools::move_to_device(_vertices.data(), _vertex_buffer);
    BufferTools::move_to_device(_indices.data(), _index_buffer);
}

void Cube::destroy_buffers() {
    BufferTools::destroy_buffer(_vertex_buffer);
    BufferTools::destroy_buffer(_index_buffer);
}

Cube::Cube(const float scale, const float tile) :
    _vertices {
        // front face
        {{ -scale, -scale,  scale, 1.0f }, { 0.0f * tile, 1.0f * tile }},
        {{  scale, -scale,  scale, 1.0f }, { 1.0f * tile, 1.0f * tile }},
        {{  scale,  scale,  scale, 1.0f }, { 1.0f * tile, 0.0f * tile }},
        {{ -scale,  scale,  scale, 1.0f }, { 0.0f * tile, 0.0f * tile }},

        // back face
        {{ -scale, -scale, -scale, 1.0f }, { 1.0f * tile, 1.0f * tile }},
        {{  scale, -scale, -scale, 1.0f }, { 0.0f * tile, 1.0f * tile }},
        {{  scale,  scale, -scale, 1.0f }, { 0.0f * tile, 0.0f * tile }},
        {{ -scale,  scale, -scale, 1.0f }, { 1.0f * tile, 0.0f * tile }},

        // left face
        {{ -scale, -scale, -scale, 1.0f }, { 0.0f * tile, 1.0f * tile }},
        {{ -scale, -scale,  scale, 1.0f }, { 1.0f * tile, 1.0f * tile }},
        {{ -scale,  scale,  scale, 1.0f }, { 1.0f * tile, 0.0f * tile }},
        {{ -scale,  scale, -scale, 1.0f }, { 0.0f * tile, 0.0f * tile }},

        // right face
        {{  scale, -scale,  scale, 1.0f }, { 0.0f * tile, 1.0f * tile }},
        {{  scale, -scale, -scale, 1.0f }, { 1.0f * tile, 1.0f * tile }},
        {{  scale,  scale, -scale, 1.0f }, { 1.0f * tile, 0.0f * tile }},
        {{  scale,  scale,  scale, 1.0f }, { 0.0f * tile, 0.0f * tile }},

        // bottom face
        {{  scale, -scale,  scale, 1.0f }, { 0.0f * tile, 1.0f * tile }},
        {{ -scale, -scale,  scale, 1.0f }, { 1.0f * tile, 1.0f * tile }},
        {{ -scale, -scale, -scale, 1.0f }, { 1.0f * tile, 0.0f * tile }},
        {{  scale, -scale, -scale, 1.0f }, { 0.0f * tile, 0.0f * tile }},

        // top face
        {{ -scale,  scale,  scale, 1.0f }, { 0.0f * tile, 1.0f * tile }},
        {{  scale,  scale,  scale, 1.0f }, { 1.0f * tile, 1.0f * tile }},
        {{  scale,  scale, -scale, 1.0f }, { 1.0f * tile, 0.0f * tile }},
        {{ -scale,  scale, -scale, 1.0f }, { 0.0f * tile, 0.0f * tile }},
    },
    _indices {
        0u,   1u,  2u,    2u,  3u,  0u,
        6u,   5u,  4u,    4u,  7u,  6u,
        8u,   9u, 10u,   10u, 11u,  8u,
        12u, 13u, 14u,   14u, 15u, 12u,
        16u, 17u, 18u,   18u, 19u, 16u,
        20u, 21u, 22u,   22u, 23u, 20u
    }
{ }

} // namespace vkl