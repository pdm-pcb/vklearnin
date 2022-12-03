#ifndef VKLEARNIN_TOOLS_MATH_HPP
#define VKLEARNIN_TOOLS_MATH_HPP

#define GLM_CXX_17
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

namespace vkl {
namespace math {

const auto ident_mat4   = glm::mat4 { 1.0f };
const auto up_vec3      = glm::vec3 { 0.0f, 1.0f,  0.0f };
const auto right_vec3   = glm::vec3 { 1.0f, 0.0f,  0.0f };
const auto forward_vec3 = glm::vec3 { 0.0f, 0.0f, -1.0f };

static constexpr float pi            = 3.14159265f;
static constexpr float pi_over_180   = pi / 180.0f;
static constexpr float pi_over_two   = pi / 2.0f;
static constexpr float pi_over_four  = pi / 4.0f;

} // namespace math
} // namespace vkl
#endif // VKLEARNIN_TOOLS_MATH_HPP