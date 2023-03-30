#ifndef VKLEARNIN_MESHES_SKYBOX_HPP
#define VKLEARNIN_MESHES_SKYBOX_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/GeneratedMesh.hpp"

namespace vkl {

class Skybox final : public GeneratedMesh {
public:
    void init(float const scale);

    Skybox() = default;
    ~Skybox() = default;

    Skybox(Skybox &&) = delete;
    Skybox(Skybox const&) = delete;

    Skybox& operator=(Skybox &&) = delete;
    Skybox& operator=(Skybox const&) = delete;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_SKYBOX_HPP