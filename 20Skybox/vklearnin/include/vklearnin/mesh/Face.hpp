#ifndef VKLEARNIN_MESH_FACE_HPP
#define VKLEARNIN_MESH_FACE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

using Index = uint32_t;

struct Face {
    Index a = std::numeric_limits<Index>::max();
    Index b = std::numeric_limits<Index>::max();
    Index c = std::numeric_limits<Index>::max();
};

} // namespace vkl

#endif // VKLEARNIN_MESH_FACE_HPP