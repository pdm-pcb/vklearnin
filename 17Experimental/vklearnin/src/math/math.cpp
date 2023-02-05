#include "vklearnin/vklearnin.hpp"
#include "vklearnin/math/math.hpp"

namespace vkl {

//==============================================================================
// Order-dependent statics

Vec3 const Vec3::unit_x { 1.0f, 0.0f, 0.0f };
Vec3 const Vec3::unit_y { 0.0f, 1.0f, 0.0f };
Vec3 const Vec3::unit_z { 0.0f, 0.0f, 1.0f };
Vec3 const Vec3::origin { 0.0f, 0.0f, 0.0f };

Vec4 const Vec4::unit_x { Vec3::unit_x, 0.0f };
Vec4 const Vec4::unit_y { Vec3::unit_y, 0.0f };
Vec4 const Vec4::unit_z { Vec3::unit_z, 0.0f };
Vec4 const Vec4::origin { Vec3::origin, 1.0f };

Mat3 const Mat3::identity { };

Mat4 const Mat4::identity { };

namespace math {

// =============================================================================
// Three-component vectors

float dot(Vec3 const& a, Vec3 const& b) {
    return a.x * b.x +
           a.y * b.y +
           a.z * b.z;
}

// -----------------------------------------------------------------------------
Vec3 cross(Vec3 const& a, Vec3 const& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

// -----------------------------------------------------------------------------
void normalize(Vec3 &a) {
    float const length = math::length(a);
    a.x /= length;
    a.y /= length;
    a.z /= length;
}

// -----------------------------------------------------------------------------
Vec3 normalized(Vec3 const& a) {
    auto result = a;
    normalize(result);
    return result;
}

// =============================================================================
// 4x4 matrices

void translate(Mat4 &a, Vec3 const& pos) {
    a.rows[3] = { pos, 1.0f };
}

// -----------------------------------------------------------------------------
Mat4 translated(Mat4 const& a, Vec3 const& pos) {
    auto result = a;
    translate(result, pos);
    return result;
}

// -----------------------------------------------------------------------------
void rotate(Mat4 &a, Vec3 const& degrees) {
    float const x_radians = math::to_radians(degrees.x);
    float const y_radians = math::to_radians(degrees.y);
    float const z_radians = math::to_radians(degrees.z);

    Mat4 rot_x = Mat4::identity;
    Mat4 rot_y = Mat4::identity;
    Mat4 rot_z = Mat4::identity;

    if(x_radians > math::float_epsilon) {
        float const c_x = std::cosf(x_radians);
        float const s_x = std::sinf(x_radians);

        rot_x.rows[1].y =  c_x;
        rot_x.rows[1].z =  s_x;
        rot_x.rows[2].y = -s_x;
        rot_x.rows[2].z =  c_x;
    }

    if(y_radians > math::float_epsilon) {
        float const c_y = std::cosf(y_radians);
        float const s_y = std::sinf(y_radians);

        rot_y.rows[0].x =  c_y;
        rot_y.rows[0].z = -s_y;
        rot_y.rows[2].x =  s_y;
        rot_y.rows[2].z =  c_y;
    }

    if(z_radians > math::float_epsilon) {
        float const c_z = std::cosf(z_radians);
        float const s_z = std::sinf(z_radians);

        rot_z.rows[0].x =  c_z;
        rot_z.rows[0].y =  s_z;
        rot_z.rows[1].x = -s_z;
        rot_z.rows[1].y =  c_z;
    }

    a *= rot_x * rot_y * rot_z;
}

// -----------------------------------------------------------------------------
Mat4 rotated(Mat4 const& a, Vec3 const& degrees) {
    auto result = a;
    rotate(result, degrees);
    return result;
}

// -----------------------------------------------------------------------------
void scale(Mat4 &a, Vec3 const& scale) {
    const Mat4 scaled_mat {
        { scale.x, 0.0f,    0.0f,    0.0f },
        { 0.0f,    scale.y, 0.0f,    0.0f },
        { 0.0f,    0.0f,    scale.z, 0.0f },
        { 0.0f,    0.0f,    0.0f,    1.0f },
    };

    a *= scaled_mat;
}

// -----------------------------------------------------------------------------
Mat4 scaled(Mat4 const& a, Vec3 const& scale) {
    auto result = a;
    math::scale(result, scale);
    return result;
}

} // namespace math
} // namespace vkl