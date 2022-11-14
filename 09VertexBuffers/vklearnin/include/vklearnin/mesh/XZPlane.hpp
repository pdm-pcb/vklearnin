#ifndef VKLEARNIN_MESH_XZPLANE_HPP
#define VKLEARNIN_MESH_XZPLANE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/mesh/Vertex.hpp"
#include "vklearnin/buffers/BufferObject.hpp"

namespace vkl {

class LogicalDevice;

class XZPlane final {
public:
    void create_buffers();
    void destroy_buffers();

    void set_corner_colors(const std::array<std::array<float, 4>, 4> &colors);

    inline const auto & vertex_buffer() const { return _vertex_buffer; }
    inline auto & vertices() { return _vertices; }

    explicit XZPlane(const LogicalDevice &logical_device);
    ~XZPlane() = default;

    XZPlane() = delete;

    XZPlane(XZPlane &&) = delete;
    XZPlane(const XZPlane &) = delete;

    XZPlane & operator=(XZPlane &&) = delete;
    XZPlane & operator=(const XZPlane &) = delete;

private:
    std::vector<Vertex> _vertices;
    BufferObject        _vertex_buffer;

    const LogicalDevice &_logical_device;
};

} // namespace vkl

#endif // VKLEARNIN_MESH_XZPLANE_HPP