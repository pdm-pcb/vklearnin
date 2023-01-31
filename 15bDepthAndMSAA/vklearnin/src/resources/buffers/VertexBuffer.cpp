#include "vklearnin/vklearnin.hpp"
#include "vklearnin/resources/buffers/VertexBuffer.hpp"

#include "vklearnin/resources/buffers/BufferTools.hpp"

namespace vkl {

// =============================================================================
void VertexBuffer::init(const size_t size) {
    _buffer.size = size;
    BufferTools::create(
        _buffer,
        (vk::BufferUsageFlagBits::eVertexBuffer |
         vk::BufferUsageFlagBits::eTransferDst),
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );
}

// =============================================================================
void VertexBuffer::shutdown() {
    BufferTools::destroy(_buffer);
}

// =============================================================================
void VertexBuffer::populate_buffer(const std::vector<Vertex> &vertices) {
    BufferTools::host_to_device(_buffer, vertices.data());
}

// =============================================================================
VertexBuffer::VertexBuffer() :
    _buffer { }
{ }

} // namespace vkl