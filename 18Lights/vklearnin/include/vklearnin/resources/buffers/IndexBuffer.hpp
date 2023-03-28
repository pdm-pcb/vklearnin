#ifndef VKLEARNIN_RESOURCES_BUFFERS_INDEXBUFFER_HPP
#define VKLEARNIN_RESOURCES_BUFFERS_INDEXBUFFER_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/Index.hpp"
#include "vklearnin/resources/buffers/BufferObject.hpp"

namespace vkl {

class IndexBuffer {
public:
    void create(size_t const size);
    void destroy();

    void populate_buffer(std::vector<Index> const &indices);

    inline auto const & buffer() const { return _buffer; }

    IndexBuffer();
    ~IndexBuffer() = default;

    IndexBuffer(IndexBuffer &&) = delete;
    IndexBuffer(const IndexBuffer &) = delete;

    IndexBuffer& operator=(IndexBuffer &&) = delete;
    IndexBuffer& operator=(const IndexBuffer &) = delete;

private:
    BufferObject _buffer;
};

} // namespace vkl

#endif // VKLEARNIN_RESOURCES_BUFFERS_INDEXBUFFER_HPP