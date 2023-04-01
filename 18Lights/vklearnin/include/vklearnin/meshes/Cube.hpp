#ifndef VKLEARNIN_MESHES_CUBE_HPP
#define VKLEARNIN_MESHES_CUBE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/GeneratedMesh.hpp"

namespace vkl {

class Cube final : public GeneratedMesh {
public:
    void init(float const scale = 1.0f,
              Vec4 const &color = { color::white, 1.0f },
              float const tile  = 1.0f);

    Cube() = default;
    ~Cube() = default;

    Cube(Cube &&) = delete;
    Cube(Cube const&) = delete;

    Cube& operator=(Cube &&) = delete;
    Cube& operator=(Cube const&) = delete;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_CUBE_HPP