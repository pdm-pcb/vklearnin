#include "vklearnin/vklearnin.hpp"
#include "vklearnin/math/math.hpp"

namespace vkl {

//==============================================================================
// Order-dependent statics

const Vec3 Vec3::unit_x { 1.0f, 0.0f, 0.0f };
const Vec3 Vec3::unit_y { 0.0f, 1.0f, 0.0f };
const Vec3 Vec3::unit_z { 0.0f, 0.0f, 1.0f };
const Vec3 Vec3::origin { 0.0f, 0.0f, 0.0f };

const Vec4 Vec4::unit_x { Vec3::unit_x, 0.0f };
const Vec4 Vec4::unit_y { Vec3::unit_y, 0.0f };
const Vec4 Vec4::unit_z { Vec3::unit_z, 0.0f };
const Vec4 Vec4::origin { Vec3::origin, 1.0f };

const Mat3 Mat3::identity { };

const Mat4 Mat4::identity { };

namespace math {

// =============================================================================
// Three-component vectors
float dot(const Vec3 &a, const Vec3 &b) {
    return a.x * b.x +
           a.y * b.y +
           a.z * b.z;
}

// -----------------------------------------------------------------------------
Vec3 cross(const Vec3 &a, const Vec3 &b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

// -----------------------------------------------------------------------------
Vec3 normalized(const Vec3 &a) {
    const float length = math::length(a);
    return {
        a.x / length,
        a.y / length,
        a.z / length
    };
}

// =============================================================================
// 3x3 matrices
Mat3 transposed(const Mat3 &a) {
    return Mat3 {
        { a.rows[0].x, a.rows[1].x, a.rows[2].x },
        { a.rows[0].y, a.rows[1].y, a.rows[2].y },
        { a.rows[0].z, a.rows[1].z, a.rows[2].z }
    };
}

// =============================================================================
// 4x4 matrices
Mat4 transposed(const Mat4 &a) {
    return Mat4 {
        { a.rows[0].x, a.rows[1].x, a.rows[2].x, a.rows[3].x },
        { a.rows[0].y, a.rows[1].y, a.rows[2].y, a.rows[3].w },
        { a.rows[0].z, a.rows[1].z, a.rows[2].z, a.rows[3].z },
        { a.rows[0].w, a.rows[1].w, a.rows[2].x, a.rows[3].w }
    };
}

// -----------------------------------------------------------------------------
Mat4 translated(const Mat4 &a, const Vec3 &pos) {
    return Mat4 {
        a.rows[0],
        a.rows[1],
        a.rows[2],
        { pos.x, pos.y, pos.z, 1.0f }
    };
}

// -----------------------------------------------------------------------------
Mat4 rotated(const Mat4 &a, const Vec3 &degrees) {
    const float x_radians = math::to_radians(degrees.x);
    const float y_radians = math::to_radians(degrees.y);
    const float z_radians = math::to_radians(degrees.z);

// #ifdef VKL_USE_GLM
//     const auto rot_x = glm::rotate(
//         glm::mat4(1.0f),
//         x_radians,
//         { 1.0f, 0.0f, 0.0f }
//     );
//     const auto rot_y = glm::rotate(
//         glm::mat4(1.0f),
//         y_radians,
//         { 0.0f, 1.0f, 0.0f }
//     );
//     const auto rot_z = glm::rotate(
//         glm::mat4(1.0f),
//         z_radians,
//         { 0.0f, 0.0f, 1.0f }
//     );

//     return Mat4(rot_x * rot_y * rot_z);
// #else
    Mat4 rot_x = Mat4::identity;
    Mat4 rot_y = Mat4::identity;
    Mat4 rot_z = Mat4::identity;

    if(x_radians >= math::float_epsilon) {
        rot_x.rows[1].y =  std::cosf(x_radians);
        rot_x.rows[1].z =  std::sinf(x_radians);
        rot_x.rows[2].y = -std::sinf(x_radians);
        rot_x.rows[2].z =  std::cosf(x_radians);
    }

    if(y_radians >= math::float_epsilon) {
        rot_y.rows[0].x =  std::cosf(y_radians);
        rot_y.rows[0].z = -std::sinf(y_radians);
        rot_y.rows[2].x =  std::sinf(y_radians);
        rot_y.rows[2].z =  std::cosf(y_radians);
    }

    if(z_radians >= math::float_epsilon) {
        rot_z.rows[0].x =  std::cosf(z_radians);
        rot_z.rows[0].y =  std::sinf(z_radians);
        rot_z.rows[1].x = -std::sinf(z_radians);
        rot_z.rows[1].y =  std::cosf(z_radians);
    }

    return a * rot_x * rot_y * rot_z;
// #endif // VKL_USE_GLM
}

} // namespace math
} // namespace vkl