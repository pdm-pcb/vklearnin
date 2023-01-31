#ifndef VKLEARNIN_RESOURCES_BUFFERS_INDEXBUFFER_HPP
#define VKLEARNIN_RESOURCES_BUFFERS_INDEXBUFFER_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/Index.hpp"
#include "vklearnin/resources/buffers/BufferTools.hpp"

namespace vkl {

class IndexBuffer {
public:
    void init(const size_t size);
    void shutdown();

    void populate_buffer(const std::vector<Index> &indices);

    inline const auto& native() const { return _buffer.handle; }

    IndexBuffer();
    ~IndexBuffer() = default;

    IndexBuffer(IndexBuffer &&) = delete;
    IndexBuffer(const IndexBuffer &) = delete;

    IndexBuffer& operator=(IndexBuffer &&) = delete;
    IndexBuffer& operator=(const IndexBuffer &) = delete;

private:
    Buffer _buffer;
};

} // namespace vkl

#endif // VKLEARNIN_RESOURCES_BUFFERS_INDEXBUFFER_HPP