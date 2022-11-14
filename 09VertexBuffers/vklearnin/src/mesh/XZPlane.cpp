#include "vklearnin/vklearnin.hpp"
#include "vklearnin/mesh/XZPlane.hpp"

#include "vklearnin/rendering/PhysicalDevice.hpp"
#include "vklearnin/rendering/LogicalDevice.hpp"

namespace vkl {

void XZPlane::create_buffers() {
    auto vertex_buffer_size = _vertices.size() * sizeof(Vertex);

    _vertex_buffer = BufferTools::create_buffer(
        vertex_buffer_size,
        vk::BufferUsageFlagBits::eVertexBuffer,
        vk::SharingMode::eExclusive,
        (vk::MemoryPropertyFlagBits::eHostVisible |
         vk::MemoryPropertyFlagBits::eHostCoherent),
        _logical_device.native()
    );

    BufferTools::fill_buffer(_vertices.data(), _vertex_buffer,
                             _logical_device);
}

void XZPlane::set_corner_colors(
    const std::array<std::array<float, 4>, 4> &colors)
{
    _vertices = {
        {{ -0.5f,  0.5f, 0.0f, 1.0f }, colors[0] },
        {{ -0.5f, -0.5f, 0.0f, 1.0f }, colors[1] },
        {{  0.5f, -0.5f, 0.0f, 1.0f }, colors[2] },
        {{ -0.5f,  0.5f, 0.0f, 1.0f }, colors[0] },
        {{  0.5f, -0.5f, 0.0f, 1.0f }, colors[2] },
        {{  0.5f,  0.5f, 0.0f, 1.0f }, colors[3] }
    };

    BufferTools::fill_buffer(_vertices.data(), _vertex_buffer,
                             _logical_device);
}

void XZPlane::destroy_buffers() {
    BufferTools::destroy_buffer(_vertex_buffer, _logical_device);
}

XZPlane::XZPlane(const LogicalDevice &logical_device) :
    _vertices {
        {{ -0.5f,  0.5f, 0.0f, 1.0f }, { 0.25f, 0.0f,  0.0f,  1.0f }},
        {{ -0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f,  0.25f, 0.0f,  1.0f }},
        {{  0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f,  0.0f,  0.25f, 1.0f }},
        {{ -0.5f,  0.5f, 0.0f, 1.0f }, { 0.25f, 0.0f,  0.0f,  1.0f }},
        {{  0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f,  0.0f,  0.25f, 1.0f }},
        {{  0.5f,  0.5f, 0.0f, 1.0f }, { 0.25f, 0.16f, 0.0f,  1.0f }}
    },
    _logical_device  { logical_device }
{ }

} // namespace vkl