#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/Face.hpp"

namespace vkl {

// =============================================================================
Face::Face(const std::array<Index, INDICES_PER_FACE> &indices) :
    _indices { indices }
{ }

} // namespace vkl