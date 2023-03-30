#ifndef VKLEARNIN_RESOURCES_BUFFERS_VERTEXBUFFER_HPP
#define VKLEARNIN_RESOURCES_BUFFERS_VERTEXBUFFER_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/Vertex.hpp"
#include "vklearnin/resources/buffers/BufferObject.hpp"
#include "vklearnin/resources/buffers/BufferTools.hpp"

namespace vkl {

class VertexBuffer {
public:
    void create(size_t const size) {
        _buffer.size = size;
        BufferTools::create(
            _buffer,
            (vk::BufferUsageFlagBits::eVertexBuffer |
            vk::BufferUsageFlagBits::eTransferDst),
            vk::MemoryPropertyFlagBits::eDeviceLocal
        );

        _offsets.emplace_back(0u);
    }

    void destroy() {
        BufferTools::destroy(_buffer);
    }

    void populate_buffer(std::vector<Vertex> const &vertices) {
        BufferTools::host_to_device(_buffer, vertices.data());
    }

    inline auto const & buffer()  const { return _buffer;  }
    inline auto const & offsets() const { return _offsets; }

    VertexBuffer() = default;
    ~VertexBuffer() = default;

    VertexBuffer(VertexBuffer &&) = delete;
    VertexBuffer(const VertexBuffer &) = delete;

    VertexBuffer& operator=(VertexBuffer &&) = delete;
    VertexBuffer& operator=(const VertexBuffer &) = delete;

private:
    BufferObject _buffer;
    std::vector<vk::DeviceSize> _offsets;
};

} // namespace vkl

#endif // VKLEARNIN_RESOURCES_BUFFERS_VERTEXBUFFER_HPP