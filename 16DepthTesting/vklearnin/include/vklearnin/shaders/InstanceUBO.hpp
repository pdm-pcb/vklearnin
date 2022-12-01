#ifndef VKLEARNIN_SHADERS_InstanceUBO_HPP
#define VKLEARNIN_SHADERS_InstanceUBO_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct InstanceUBO {
    glm::mat4 model_matrix { 1.0f };
};

} // namespace vkl

#endif // VKLEARNIN_SHADERS_InstanceUBO_HPP