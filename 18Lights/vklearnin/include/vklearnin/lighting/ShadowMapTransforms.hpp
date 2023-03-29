#ifndef VKLEARNIN_LIGHTING_SHADOWMAPTRANSFORMS_HPP
#define VKLEARNIN_LIGHTING_SHADOWMAPTRANSFORMS_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct ShadowMapTransforms final {
    Mat4 light_vp_matrix = Mat4::identity;
};

} // namespace vkl

#endif // VKLEARNIN_LIGHTING_SHADOWMAPTRANSFORMS_HPP