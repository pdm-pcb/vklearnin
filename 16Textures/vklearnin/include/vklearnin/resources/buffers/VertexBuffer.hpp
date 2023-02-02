#ifndef VKLEARNIN_RESOURCES_BUFFERS_VERTEXBUFFER_HPP
#define VKLEARNIN_RESOURCES_BUFFERS_VERTEXBUFFER_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/buffers/BufferObject.hpp"

namespace vkl {

template <typename VertexType>
class VertexBuffer {
public:
    void init(const size_t size) {
        _buffer.size = size;
        BufferTools::create(
            _buffer,
            (vk::BufferUsageFlagBits::eVertexBuffer |
            vk::BufferUsageFlagBits::eTransferDst),
            vk::MemoryPropertyFlagBits::eDeviceLocal
        );
    }

    void shutdown() {
        BufferTools::destroy(_buffer);
    }

    void populate_buffer(const std::vector<VertexType> &vertices) {
        BufferTools::host_to_device(_buffer, vertices.data());
    }

    inline auto const& native() const { return _buffer.handle; }

    VertexBuffer() = default;
    ~VertexBuffer() = default;

    VertexBuffer(VertexBuffer &&) = delete;
    VertexBuffer(const VertexBuffer &) = delete;

    VertexBuffer& operator=(VertexBuffer &&) = delete;
    VertexBuffer& operator=(const VertexBuffer &) = delete;

private:
    BufferObject _buffer;
};

} // namespace vkl

#endif // VKLEARNIN_RESOURCES_BUFFERS_VERTEXBUFFER_HPP