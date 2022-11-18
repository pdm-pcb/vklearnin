#include "vklearnin/vklearnin.hpp"
#include "vklearnin/mesh/UnitCube.hpp"

namespace vkl {

void UnitCube::create_buffers() {
    auto vertex_buffer_size = _vertices.size() * sizeof(Vertex);
    auto index_buffer_size = _indices.size() * sizeof(uint32_t);

    _vertex_buffer = BufferTools::create_buffer(
        vertex_buffer_size,
        (vk::BufferUsageFlagBits::eTransferDst |
         vk::BufferUsageFlagBits::eVertexBuffer),
        vk::SharingMode::eExclusive,
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    _index_buffer = BufferTools::create_buffer(
        index_buffer_size,
        (vk::BufferUsageFlagBits::eTransferDst |
         vk::BufferUsageFlagBits::eIndexBuffer),
        vk::SharingMode::eExclusive,
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    BufferTools::fill_buffer(_vertices.data(), _vertex_buffer);
    BufferTools::fill_buffer(_indices.data(), _index_buffer);
}

void UnitCube::destroy_buffers() {
    BufferTools::destroy_buffer(_vertex_buffer);
    BufferTools::destroy_buffer(_index_buffer);
}

UnitCube::UnitCube() :
    _vertices {
        // front face
        {{ -0.5f, -0.5f,  0.5f, 1.0f }, { 0.5f, 0.0f,  0.0f,  1.0f }},
        {{  0.5f, -0.5f,  0.5f, 1.0f }, { 0.5f, 0.0f,  0.0f,  1.0f }},
        {{  0.5f,  0.5f,  0.5f, 1.0f }, { 0.5f, 0.0f,  0.0f,  1.0f }},
        {{ -0.5f,  0.5f,  0.5f, 1.0f }, { 0.5f, 0.0f,  0.0f,  1.0f }},

        // back face
        {{ -0.5f, -0.5f, -0.5f, 1.0f }, { 0.0f,  0.5f, 0.0f,  1.0f }},
        {{  0.5f, -0.5f, -0.5f, 1.0f }, { 0.0f,  0.5f, 0.0f,  1.0f }},
        {{  0.5f,  0.5f, -0.5f, 1.0f }, { 0.0f,  0.5f, 0.0f,  1.0f }},
        {{ -0.5f,  0.5f, -0.5f, 1.0f }, { 0.0f,  0.5f, 0.0f,  1.0f }},

        // left face
        {{ -0.5f, -0.5f, -0.5f, 1.0f }, { 0.0f,  0.0f,  0.5f, 1.0f }},
        {{ -0.5f, -0.5f,  0.5f, 1.0f }, { 0.0f,  0.0f,  0.5f, 1.0f }},
        {{ -0.5f,  0.5f,  0.5f, 1.0f }, { 0.0f,  0.0f,  0.5f, 1.0f }},
        {{ -0.5f,  0.5f, -0.5f, 1.0f }, { 0.0f,  0.0f,  0.5f, 1.0f }},

        // right face
        {{  0.5f, -0.5f,  0.5f, 1.0f }, { 0.5f, 0.38f, 0.0f,  1.0f }},
        {{  0.5f, -0.5f, -0.5f, 1.0f }, { 0.5f, 0.38f, 0.0f,  1.0f }},
        {{  0.5f,  0.5f, -0.5f, 1.0f }, { 0.5f, 0.38f, 0.0f,  1.0f }},
        {{  0.5f,  0.5f,  0.5f, 1.0f }, { 0.5f, 0.38f, 0.0f,  1.0f }},

        // bottom face
        {{  0.5f, -0.5f,  0.5f, 1.0f }, { 0.0f, 0.5f, 0.38f,  1.0f }},
        {{ -0.5f, -0.5f,  0.5f, 1.0f }, { 0.0f, 0.5f, 0.38f,  1.0f }},
        {{ -0.5f, -0.5f, -0.5f, 1.0f }, { 0.0f, 0.5f, 0.38f,  1.0f }},
        {{  0.5f, -0.5f, -0.5f, 1.0f }, { 0.0f, 0.5f, 0.38f,  1.0f }},

        // top face
        {{ -0.5f,  0.5f,  0.5f, 1.0f }, { 0.38f, 0.0f, 0.5f,  1.0f }},
        {{  0.5f,  0.5f,  0.5f, 1.0f }, { 0.38f, 0.0f, 0.5f,  1.0f }},
        {{  0.5f,  0.5f, -0.5f, 1.0f }, { 0.38f, 0.0f, 0.5f,  1.0f }},
        {{ -0.5f,  0.5f, -0.5f, 1.0f }, { 0.38f, 0.0f, 0.5f,  1.0f }},
        
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