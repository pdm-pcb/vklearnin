#include "vklearnin/vklearnin.hpp"
#include "vklearnin/math/math.hpp"

namespace vkl {

// =============================================================================
// Four-component vectors
float dot(const Vec4 &a, const Vec4 &b) {
    return a.x * b.x +
           a.y * b.y +
           a.z * b.z;
}

// -----------------------------------------------------------------------------
Vec4 cross(const Vec4 &a, const Vec4 &b) {
    if(a.w != b.w) {
        CONSOLE_CRITICAL(
            "Cannot take the cross product of a vector and a point."
        );
        return { };
    }

    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
        a.w
    };
}

// -----------------------------------------------------------------------------
Vec4 normalized(const Vec4 &a) {
    const float length = calc_length(a);
    Vec4 result {
        a.x / length,
        a.y / length,
        a.z / length,
        a.w
    };

    return result;
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
Mat4 translated(const Mat4 &a, Vec4 &pos) {
    return Mat4 { a.rows[0], a.rows[1], a.rows[2], pos };
}

// -----------------------------------------------------------------------------
Mat4 rotated(const Mat4 &a, const Vec4 &degrees) {
    const float x_radians = math::degrees_to_radians(degrees.x);
    Mat4 rot_x = Mat4::identity;
    rot_x.rows[1].y =  std::cosf(x_radians);
    rot_x.rows[1].z =  std::sinf(x_radians);
    rot_x.rows[2].y = -std::sinf(x_radians);
    rot_x.rows[2].z =  std::cosf(x_radians);

    const float y_radians = math::degrees_to_radians(degrees.y);
    Mat4 rot_y = Mat4::identity;
    rot_y.rows[0].x =  std::cosf(y_radians);
    rot_y.rows[0].z = -std::sinf(y_radians);
    rot_y.rows[2].x =  std::sinf(y_radians);
    rot_y.rows[2].z =  std::cosf(y_radians);

    const float z_radians = math::degrees_to_radians(degrees.z);
    Mat4 rot_z = Mat4::identity;
    rot_z.rows[0].x =  std::cosf(z_radians);
    rot_z.rows[0].y =  std::sinf(z_radians);
    rot_z.rows[1].x = -std::sinf(z_radians);
    rot_z.rows[1].y =  std::cosf(z_radians);

    return a * rot_x * rot_y * rot_z;
}

} // namespace vkl