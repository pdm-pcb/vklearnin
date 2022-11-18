#include "vklearnin/vklearnin.hpp"
#include "vklearnin/mesh/XZUnitPlane.hpp"

namespace vkl {

void XZUnitPlane::create_buffers() {
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

void XZUnitPlane::set_corner_colors(
    const std::array<std::array<float, 4>, 4> &colors)
{
    _vertices = {
        {{ -0.5f,  0.5f, 0.0f, 1.0f }, colors[0] },
        {{ -0.5f, -0.5f, 0.0f, 1.0f }, colors[1] },
        {{  0.5f, -0.5f, 0.0f, 1.0f }, colors[2] },
        {{  0.5f,  0.5f, 0.0f, 1.0f }, colors[3] }
    };

    BufferTools::fill_buffer(_vertices.data(), _vertex_buffer);
}

void XZUnitPlane::destroy_buffers() {
    BufferTools::destroy_buffer(_vertex_buffer);
    BufferTools::destroy_buffer(_index_buffer);
}

XZUnitPlane::XZUnitPlane() :
    _vertices {
        {{ -0.5f, -0.5f,  0.5f, 1.0f }, { 0.5f, 0.0f,  0.0f,  1.0f }},
        {{  0.5f, -0.5f,  0.5f, 1.0f }, { 0.5f, 0.0f,  0.0f,  1.0f }},
        {{  0.5f,  0.5f,  0.5f, 1.0f }, { 0.5f, 0.0f,  0.0f,  1.0f }},
        {{ -0.5f,  0.5f,  0.5f, 1.0f }, { 0.5f, 0.0f,  0.0f,  1.0f }},
    },
    _indices { 0u, 1u, 2u, 2u, 3u, 0u }
{ }

} // namespace vkl