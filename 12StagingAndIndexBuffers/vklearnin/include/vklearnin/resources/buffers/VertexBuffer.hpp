#ifndef VKLEARNIN_RESOURCES_BUFFERS_VERTEXBUFFER_HPP
#define VKLEARNIN_RESOURCES_BUFFERS_VERTEXBUFFER_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/Vertex.hpp"
#include "vklearnin/resources/buffers/BufferTools.hpp"

namespace vkl {

class VertexBuffer {
public:
    void init(const size_t size);
    void shutdown();

    void populate_buffer(const std::vector<Vertex> &vertices);

    inline auto native() const { return _buffer.handle; }

    VertexBuffer();
    ~VertexBuffer() = default;

    VertexBuffer(VertexBuffer &&) = delete;
    VertexBuffer(const VertexBuffer &) = delete;

    VertexBuffer & operator=(VertexBuffer &&) = delete;
    VertexBuffer & operator=(const VertexBuffer &) = delete;

private:
    Buffer _buffer;
};

} // namespace vkl

#endif // VKLEARNIN_RESOURCES_BUFFERS_VERTEXBUFFER_HPP