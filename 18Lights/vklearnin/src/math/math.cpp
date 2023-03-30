#include "vklearnin/vklearnin.hpp"
#include "vklearnin/math/math.hpp"

namespace vkl {

// Insist on keeping the math types simple
static_assert(sizeof(Vec4) ==  sizeof(float) * 4);
static_assert(sizeof(Mat4) ==  sizeof(Vec4) * 4);
static_assert(std::is_aggregate<Vec4>());
static_assert(std::is_aggregate<Mat4>());

// Order-dependent static variables
Vec4 const Vec4::unit_x { 1.0f, 0.0f, 0.0f, 0.0f };
Vec4 const Vec4::unit_y { 0.0f, 1.0f, 0.0f, 0.0f };
Vec4 const Vec4::unit_z { 0.0f, 0.0f, 1.0f, 0.0f };
Vec4 const Vec4::origin { 0.0f, 0.0f, 0.0f, 1.0f };

Mat4 const Mat4::identity { };
Mat4 const Mat4::zero {
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
};

namespace math {

// =============================================================================
// Vector operations
Vec4 normalize(Vec4 const &v) {
    auto length = length2(v);
    if(length <= 0.0f || length == 1.0f) {
        return v;
    }

    auto result = v;
    float const length_inv = 1.0f / std::sqrt(length);

    result.x *= length_inv;
    result.y *= length_inv;
    result.z *= length_inv;

    return result;
}

float dot(Vec4 const &a, Vec4 const &b) {
    return a.x * b.x +
           a.y * b.y +
           a.z * b.z;
}

Vec4 cross(Vec4 const &a, Vec4 const &b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
        0.0f
    };
}

// =============================================================================
// Matrix operations
Mat4 transpose(Mat4 const &m) {
    auto result = m;

    std::swap(result.x.y, result.y.x);
    std::swap(result.x.z, result.z.x);
    std::swap(result.x.w, result.w.x);

    std::swap(result.y.z, result.z.y);
    std::swap(result.y.w, result.w.y);

    std::swap(result.z.w, result.w.z);

    return result;
}

Mat4 translate(Mat4 const &m, Vec4 const &v) {
    auto result = m;
    result.w = (m.x * v.x) + (m.y * v.y) + (m.z * v.z) + m.w;
    return result;
}

Mat4 rotate(Mat4 const &m, float const angle, Vec4 const &axis) {
    auto result = m;
    float const theta = radians(angle);
    float const cos_theta = std::cos(theta);
    float const sin_theta = std::sin(theta);

    auto const rot_axis = normalize(axis);
    auto const temp = (1.0f - cos_theta) * rot_axis;

    auto rot_mat = Mat4::identity;
    rot_mat.x.x = cos_theta + temp.x * rot_axis.x;
    rot_mat.x.y = temp.x * rot_axis.y + sin_theta * rot_axis.z;
    rot_mat.x.z = temp.x * rot_axis.z - sin_theta * rot_axis.y;

    rot_mat.y.x = temp.y * rot_axis.x - sin_theta * rot_axis.z;
    rot_mat.y.y = cos_theta + temp.y * rot_axis.y;
    rot_mat.y.z = temp.y * rot_axis.z + sin_theta * rot_axis.x;

    rot_mat.z.x = temp.z * rot_axis.x + sin_theta * rot_axis.y;
    rot_mat.z.y = temp.z * rot_axis.y - sin_theta * rot_axis.x;
    rot_mat.z.z = cos_theta + temp.z * rot_axis.z;

    result.x = m.x * rot_mat.x.x + m.y * rot_mat.x.y + m.z * rot_mat.x.z;
    result.y = m.x * rot_mat.y.x + m.y * rot_mat.y.y + m.z * rot_mat.y.z;
    result.z = m.x * rot_mat.z.x + m.y * rot_mat.z.y + m.z * rot_mat.z.z;
    result.w = m.w;

    return result;
}

Mat4 scale(Mat4 const &m, float const &pct) {
    Mat4 result;
    result.x = m.x * pct;
    result.y = m.y * pct;
    result.z = m.z * pct;
    result.w = m.w;

    return result;
}

// =============================================================================
// Camera math
// Credit to:
// https://nlguillemot.wordpress.com/2016/12/07/reversed-z-in-opengl/
// https://www.danielecarbone.com/reverse-depth-buffer-in-opengl/
// https://discourse.nphysics.org/t/reversed-z-and-infinite-zfar-in-projections/341

Mat4 ortho_proj_rh_zo(float const near, float const far,
                      float const left, float const right,
                      float const bottom, float const top)
{
    auto result = Mat4::identity;

    auto const a = right - left;
    auto const b = top - bottom;
    auto const c = far - near;

    result.x.x = 2.0f / a;
    result.y.y = -2.0f / b;
    result.z.z = -1.0f / c;
    result.w.x = -(right + left) / a;
    result.w.y = -(top + bottom) / b;
    result.w.z = -near / c;

    return result;
}

Mat4 persp_proj_rh_zo_inf(float const near, float const vfov_degrees,
                          float const aspect_ratio)
{
    auto result = Mat4::zero;

    auto const fov_rad = math::radians(vfov_degrees);
    auto const half_angle = std::tan(fov_rad * 0.5f);
    auto const half_angle_recip = 1.0f / half_angle;

    result.x.x = half_angle_recip / aspect_ratio;
    result.y.y = -half_angle_recip;
    result.z.z = -1.0f;
    result.z.w = -1.0f;
    result.w.z = -near;

    return result;
}

Mat4 persp_proj_rh_oz_inf(float const near, float const vfov_degrees,
                          float const aspect_ratio)
{
    auto result = Mat4::zero;

    auto const fov_rad = math::radians(vfov_degrees);
    auto const half_angle = std::tan(fov_rad * 0.5f);
    auto const half_angle_recip = 1.0f / half_angle;

    result.x.x = half_angle_recip / aspect_ratio;
    result.y.y = -half_angle_recip;
    result.z.w = -1.0f;
    result.w.z = near;

    return result;
}

Mat4 orient_view_matrix(Vec4 const &position, Vec4 const &forward,
                        Vec4 const &side, Vec4 const &up)
{
    auto result = Mat4::identity;

    result.x.x = side.x;
    result.y.x = side.y;
    result.z.x = side.z;
    result.x.y = up.x;
    result.y.y = up.y;
    result.z.y = up.z;
    result.x.z = -forward.x;
    result.y.z = -forward.y;
    result.z.z = -forward.z;
    result.w.x = -math::dot(side, position);
    result.w.y = -math::dot(up, position);
    result.w.z =  math::dot(forward, position);

    return result;
}

Mat4 look_at(Vec4 const &position, Vec4 const &target, Vec4 const &up) {
    auto const forward = normalize(target - position);
    auto const side    = normalize(cross(up, forward));
    auto const new_up  = cross(forward, side);

    return orient_view_matrix(position, forward, side, new_up);
}

} // namespace math
} // namespace vkl