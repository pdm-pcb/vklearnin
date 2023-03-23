#ifndef VKLEARNIN_RESOURCES_BUFFERS_VERTEXBUFFER_HPP
#define VKLEARNIN_RESOURCES_BUFFERS_VERTEXBUFFER_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/buffers/BufferObject.hpp"
#include "vklearnin/resources/buffers/BufferTools.hpp"

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

        _offsets.emplace_back(0u);
    }

    void shutdown() {
        BufferTools::destroy(_buffer);
    }

    void populate_buffer(const std::vector<VertexType> &vertices) {
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