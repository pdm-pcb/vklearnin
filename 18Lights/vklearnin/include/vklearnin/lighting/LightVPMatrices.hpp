#ifndef VKLEARNIN_LIGHTING_LIGHTVPMATRICES_HPP
#define VKLEARNIN_LIGHTING_LIGHTVPMATRICES_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct LightVPMatrices final {
    Mat4 dir_vp_mat  = Mat4::identity;
    Mat4 spot_vp_mat = Mat4::identity;
};

} // namespace vkl

#endif // VKLEARNIN_LIGHTING_LIGHTVPMATRICES_HPP