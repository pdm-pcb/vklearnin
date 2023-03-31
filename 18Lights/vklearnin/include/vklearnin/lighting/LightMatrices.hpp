#ifndef VKLEARNIN_LIGHTING_LIGHTMATRICES_HPP
#define VKLEARNIN_LIGHTING_LIGHTMATRICES_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct ShadowMapTransforms final {
    Mat4 dir_vp_mat  = Mat4::identity;
    Mat4 spot_vp_mat = Mat4::identity;
};

struct ShadowPassMVP final {
    Mat4 light_vp_matrix = Mat4::identity;
    Mat4 model_matrix    = Mat4::identity;
};

} // namespace vkl

#endif // VKLEARNIN_LIGHTING_LIGHTMATRICES_HPP