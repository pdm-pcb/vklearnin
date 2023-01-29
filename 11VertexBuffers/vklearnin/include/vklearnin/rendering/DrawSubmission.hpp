#ifndef VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP
#define VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Mesh;

struct DrawSubmission {
    Mesh &mesh;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP