#ifndef VKLEARNIN_RENDERING_CAMERADATA_HPP
#define VKLEARNIN_RENDERING_CAMERADATA_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct CameraData {
    glm::mat4 view_matrix;
    glm::mat4 proj_matrix;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_CAMERADATA_HPP