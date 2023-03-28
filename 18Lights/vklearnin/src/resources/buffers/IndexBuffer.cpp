#include "vklearnin/vklearnin.hpp"
#include "vklearnin/resources/buffers/IndexBuffer.hpp"

#include "vklearnin/resources/buffers/BufferTools.hpp"

namespace vkl {

// =============================================================================
void IndexBuffer::create(size_t const size) {
    _buffer.size = size;
    BufferTools::create(
        _buffer,
        (vk::BufferUsageFlagBits::eIndexBuffer |
         vk::BufferUsageFlagBits::eTransferDst),
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );
}

// =============================================================================
void IndexBuffer::destroy() {
    BufferTools::destroy(_buffer);
}

// =============================================================================
void IndexBuffer::populate_buffer(std::vector<Index> const &indices) {
    BufferTools::host_to_device(_buffer, indices.data());
}

// =============================================================================
IndexBuffer::IndexBuffer() :
    _buffer { }
{ }

} // namespace vkl