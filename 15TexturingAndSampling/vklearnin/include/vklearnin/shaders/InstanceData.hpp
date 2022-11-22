#ifndef VKLEARNIN_SHADERS_INSTANCEDATA_HPP
#define VKLEARNIN_SHADERS_INSTANCEDATA_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct InstanceData {
    glm::mat4 model_matrix;
};

} // namespace vkl

#endif // VKLEARNIN_SHADERS_INSTANCEDATA_HPP