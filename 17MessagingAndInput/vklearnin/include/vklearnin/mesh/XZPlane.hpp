#ifndef VKLEARNIN_MESH_XZUNITPLANE_HPP
#define VKLEARNIN_MESH_XZUNITPLANE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/mesh/Vertex.hpp"
#include "vklearnin/shaders/BufferObject.hpp"

namespace vkl {

class LogicalDevice;

class XZPlane final {
public:
    void create_buffers();
    void destroy_buffers();

    inline const auto & vertex_buffer() const { return _vertex_buffer; }
    inline auto & vertices() { return _vertices; }

    inline const auto & index_buffer() const { return _index_buffer; }
    inline auto & indices() { return _indices; }

    XZPlane(const float scale, const float texture_repeat);
    ~XZPlane() = default;
    XZPlane() = delete;

    XZPlane(XZPlane &&) = delete;
    XZPlane(const XZPlane &) = delete;

    XZPlane & operator=(XZPlane &&) = delete;
    XZPlane & operator=(const XZPlane &) = delete;

private:
    std::vector<Vertex>   _vertices;
    std::vector<uint32_t> _indices;

    BufferObject _vertex_buffer;
    BufferObject _index_buffer;
};

} // namespace vkl

#endif // VKLEARNIN_MESH_XZUNITPLANE_HPP