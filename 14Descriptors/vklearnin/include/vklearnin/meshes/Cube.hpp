#ifndef VKLEARNIN_MESHES_CUBE_HPP
#define VKLEARNIN_MESHES_CUBE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/Mesh.hpp"

namespace vkl {

class Cube final : public Mesh {
public:
    using CornerColors = std::array<std::array<float, 4>, 8>;

    void init(const float scale, const CornerColors corner_colors);
    void shutdown();

    Cube() = default;
    ~Cube() = default;

    Cube(Cube &&) = delete;
    Cube(const Cube &) = delete;

    Cube& operator=(Cube &&) = delete;
    Cube& operator=(const Cube &) = delete;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_CUBE_HPP