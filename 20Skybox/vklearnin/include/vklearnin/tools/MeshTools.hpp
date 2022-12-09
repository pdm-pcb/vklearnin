#ifndef VKLEARNIN_TOOLS_MESHTOOLS_HPP
#define VKLEARNIN_TOOLS_MESHTOOLS_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/mesh/Vertex.hpp"
#include "vklearnin/mesh/Face.hpp"

namespace vkl {

namespace MeshTools {

void build_surface_normals(std::vector<Vertex> &vertices,
                           const std::vector<Face> &faces);

} // namespace MeshTools
} // namespace vkl
#endif // VKLEARNIN_TOOLS_MESHTOOLS_HPP