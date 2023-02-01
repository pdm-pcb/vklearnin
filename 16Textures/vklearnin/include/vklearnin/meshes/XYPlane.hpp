#ifndef VKLEARNIN_MESHES_XYPLANE_HPP
#define VKLEARNIN_MESHES_XYPLANE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/Mesh.hpp"

namespace vkl {

class XYPlane final : public Mesh {
public:
    void init(const float scale = 1.0f, const float tile = 1.0f);
    void shutdown();

    XYPlane() = default;
    ~XYPlane() = default;

    XYPlane(XYPlane &&) = delete;
    XYPlane(const XYPlane &) = delete;

    XYPlane& operator=(XYPlane &&) = delete;
    XYPlane& operator=(const XYPlane &) = delete;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_XYPLANE_HPP