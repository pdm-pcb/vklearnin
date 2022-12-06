#include "vklearnin/vklearnin.hpp"
#include "vklearnin/mesh/Skybox.hpp"

namespace vkl {

void Skybox::create_buffers() {
    auto vertex_buffer_size = _vertices.size() * sizeof(Vertex);
    auto index_buffer_size  = _indices.size()  * sizeof(uint32_t);

    CONSOLE_TRACE(
        "Allocating Skybox buffers: {} vb {} ib",
        vertex_buffer_size, index_buffer_size
    );

    _vertex_buffer = BufferTools::create_buffer(
        vertex_buffer_size,
        (vk::BufferUsageFlagBits::eTransferDst |
         vk::BufferUsageFlagBits::eVertexBuffer),
        vk::SharingMode::eExclusive,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        "Skybox vertex"
    );

    _index_buffer = BufferTools::create_buffer(
        index_buffer_size,
        (vk::BufferUsageFlagBits::eTransferDst |
         vk::BufferUsageFlagBits::eIndexBuffer),
        vk::SharingMode::eExclusive,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        "Skybox index"
    );

    BufferTools::move_to_device(_vertices.data(), _vertex_buffer);
    BufferTools::move_to_device(_indices.data(), _index_buffer);
}

void Skybox::destroy_buffers() {
    BufferTools::destroy_buffer(_vertex_buffer);
    BufferTools::destroy_buffer(_index_buffer);
}

Skybox::Skybox(const float scale) :
    _indices {
        0u,  1u,  2u,  2u,  3u,  0u,  6u,  5u,  4u,  4u,  7u,  6u,
        8u,  9u,  10u, 10u, 11u, 8u,  12u, 13u, 14u, 14u, 15u, 12u,
        16u, 17u, 18u, 18u, 19u, 16u, 20u, 21u, 22u, 22u, 23u, 20u,
    }
{
    _vertices = {
        {{ -scale, -scale, -scale, 1.0f }, { 0.25f, math::one_third  }},
        {{  scale, -scale, -scale, 1.0f }, { 0.5f,  math::one_third  }},
        {{  scale,  scale, -scale, 1.0f }, { 0.5f,  math::two_thirds }},
        {{ -scale,  scale, -scale, 1.0f }, { 0.25f, math::two_thirds }},
        {{ -scale, -scale,  scale, 1.0f }, { 1.0f,  math::one_third  }},
        {{  scale, -scale,  scale, 1.0f }, { 0.75f, math::one_third  }},
        {{  scale,  scale,  scale, 1.0f }, { 0.75f, math::two_thirds }},
        {{ -scale,  scale,  scale, 1.0f }, { 1.0f,  math::two_thirds }},
        {{ -scale, -scale,  scale, 1.0f }, { 0.0f,  math::one_third  }},
        {{ -scale, -scale, -scale, 1.0f }, { 0.25f, math::one_third  }},
        {{ -scale,  scale, -scale, 1.0f }, { 0.25f, math::two_thirds }},
        {{ -scale,  scale,  scale, 1.0f }, { 0.0f,  math::two_thirds }},
        {{  scale, -scale, -scale, 1.0f }, { 0.5f,  math::one_third  }},
        {{  scale, -scale,  scale, 1.0f }, { 0.75f, math::one_third  }},
        {{  scale,  scale,  scale, 1.0f }, { 0.75f, math::two_thirds }},
        {{  scale,  scale, -scale, 1.0f }, { 0.5f,  math::two_thirds }},
        {{ -scale, -scale,  scale, 1.0f }, { 0.25f, 0.0f }},
        {{  scale, -scale,  scale, 1.0f }, { 0.5f,  0.0f }},
        {{  scale, -scale, -scale, 1.0f }, { 0.5f,  math::one_third  }},
        {{ -scale, -scale, -scale, 1.0f }, { 0.25f, math::one_third  }},
        {{ -scale,  scale, -scale, 1.0f }, { 0.25f, math::two_thirds }},
        {{  scale,  scale, -scale, 1.0f }, { 0.5f,  math::two_thirds }},
        {{  scale,  scale,  scale, 1.0f }, { 0.5f,  1.0f }},
        {{ -scale,  scale,  scale, 1.0f }, { 0.25f, 1.0f }},
    };
}

} // namespace vkl