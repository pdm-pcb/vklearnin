#ifndef VKLEARNIN_MESHES_XYPLANE_HPP
#define VKLEARNIN_MESHES_XYPLANE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/GeneratedMesh.hpp"

namespace vkl {

class Plane final : public GeneratedMesh {
public:
    void init(float const scale = 1.0f,
              Vec4 const &color = Color::white,
              float const tile  = 0.0f);

    Plane() = default;
    ~Plane() = default;

    Plane(Plane &&) = delete;
    Plane(Plane const&) = delete;

    Plane& operator=(Plane &&) = delete;
    Plane& operator=(Plane const&) = delete;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_XYPLANE_HPP