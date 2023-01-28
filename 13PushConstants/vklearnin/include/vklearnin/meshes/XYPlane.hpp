#ifndef VKLEARNIN_MESHES_XYPLANE_HPP
#define VKLEARNIN_MESHES_XYPLANE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/Mesh.hpp"

namespace vkl {

class XYPlane final : public Mesh {
public:
    using CornerColors = std::array<std::array<float, 4>, 4>;

    void init(const float scale, const CornerColors corner_colors);
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