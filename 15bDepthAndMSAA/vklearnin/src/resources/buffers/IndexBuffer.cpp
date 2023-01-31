#include "vklearnin/vklearnin.hpp"
#include "vklearnin/resources/buffers/IndexBuffer.hpp"

#include "vklearnin/resources/buffers/BufferTools.hpp"

namespace vkl {

// =============================================================================
void IndexBuffer::init(const size_t size) {
    _buffer.size = size;
    BufferTools::create(
        _buffer,
        (vk::BufferUsageFlagBits::eIndexBuffer |
         vk::BufferUsageFlagBits::eTransferDst),
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );
}

// =============================================================================
void IndexBuffer::shutdown() {
    BufferTools::destroy(_buffer);
}

// =============================================================================
void IndexBuffer::populate_buffer(const std::vector<Index> &indices) {
    BufferTools::host_to_device(_buffer, indices.data());
}

// =============================================================================
IndexBuffer::IndexBuffer() :
    _buffer { }
{ }

} // namespace vkl