#ifndef VKLEARNIN_ENGINE_CameraUBO_HPP
#define VKLEARNIN_ENGINE_CameraUBO_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct CameraUBO {
    glm::mat4 view_matrix;
    glm::mat4 proj_matrix;
};

} // namespace vkl

#endif // VKLEARNIN_ENGINE_CameraUBO_HPP