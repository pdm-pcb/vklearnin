#include "vklearnin/vklearnin.hpp"
#include "vklearnin/mesh/XZPlane.hpp"

namespace vkl {

void XZPlane::create_buffers() {
    auto vertex_buffer_size = _vertices.size() * sizeof(Vertex);
    auto index_buffer_size = _indices.size() * sizeof(uint32_t);

    _vertex_buffer = BufferTools::create_buffer(
        vertex_buffer_size,
        (vk::BufferUsageFlagBits::eTransferDst |
         vk::BufferUsageFlagBits::eVertexBuffer),
        vk::SharingMode::eExclusive,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        "xz plane vertex"
    );

    _index_buffer = BufferTools::create_buffer(
        index_buffer_size,
        (vk::BufferUsageFlagBits::eTransferDst |
         vk::BufferUsageFlagBits::eIndexBuffer),
        vk::SharingMode::eExclusive,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        "xz plane index"
    );

    BufferTools::move_to_device(_vertices.data(), _vertex_buffer);
    BufferTools::move_to_device(_indices.data(), _index_buffer);
}

void XZPlane::destroy_buffers() {
    BufferTools::destroy_buffer(_vertex_buffer);
    BufferTools::destroy_buffer(_index_buffer);
}

XZPlane::XZPlane(const float scale, const float tile) :
    _vertices {
        {{ -scale, 0.0f, -scale, 1.0f }, { 0.0f * tile, 0.0f * tile }},
        {{ -scale, 0.0f,  scale, 1.0f }, { 0.0f * tile, 1.0f * tile }},
        {{  scale, 0.0f,  scale, 1.0f }, { 1.0f * tile, 1.0f * tile }},
        {{  scale, 0.0f, -scale, 1.0f }, { 1.0f * tile, 0.0f * tile }},
    },
    _indices { 0u, 1u, 2u, 2u, 3u, 0u }
{ }

} // namespace vkl