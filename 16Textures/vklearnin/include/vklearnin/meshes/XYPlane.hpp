#ifndef VKLEARNIN_MESHES_XYPLANE_HPP
#define VKLEARNIN_MESHES_XYPLANE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/Mesh.hpp"

namespace vkl {

template <typename VertexType>
class XYPlane final : public Mesh<VertexType> {
public:
    void init(float const scale = 1.0f, float const tile = 1.0f) {
        Mesh::_set_vertices({
            {{ -scale, -scale,  scale, 1.0f }, { 0.0f, tile }},
            {{  scale, -scale,  scale, 1.0f }, { tile, tile }},
            {{  scale,  scale,  scale, 1.0f }, { tile, 0.0f }},
            {{ -scale,  scale,  scale, 1.0f }, { 0.0f, 0.0f }},
        });

        Mesh::_set_indices({
            0, 1, 2,
            0, 2, 3
        });
    }

    void shutdown() {
        Mesh::_shutdown_buffers();
    }

    XYPlane() = default;
    ~XYPlane() = default;

    XYPlane(XYPlane &&) = delete;
    XYPlane(XYPlane const&) = delete;

    XYPlane& operator=(XYPlane &&) = delete;
    XYPlane& operator=(XYPlane const&) = delete;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_XYPLANE_HPP