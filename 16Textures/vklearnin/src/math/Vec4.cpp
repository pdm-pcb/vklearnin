#include "vklearnin/vklearnin.hpp"
#include "vklearnin/math/Vec4.hpp"

#include "vklearnin/math/math.hpp"

namespace vkl {

// =============================================================================
Vec4::Vec4(const std::array<float, 4> &vec) :
    x { vec[0] },
    y { vec[1] },
    z { vec[2] },
    w { vec[3] }
{ }

Vec4::Vec4(float const x, float const y, float const z, float const w) :
    x { x },
    y { y },
    z { z },
    w { w }
{ }

Vec4::Vec4() :
    x { 0.0f },
    y { 0.0f },
    z { 0.0f },
    w { 0.0f }
{ }

Vec4::Vec4(const Vec3 &other, float const w) :
    x { other.x },
    y { other.y },
    z { other.z },
    w { w }
{ }

// =============================================================================
Vec4& Vec4::operator+=(Vec4 const& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;

    return *this;
}

Vec4& Vec4::operator-=(Vec4 const& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;

    return *this;
}

Vec4& Vec4::operator*=(float const scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;

    return *this;
}

Vec4& Vec4::operator/=(float const scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;

    return *this;
}

// =============================================================================
Vec4 operator+(Vec4 const& a, Vec4 const& b) {
    auto result = a;
    return result += b;
}

Vec4 operator-(Vec4 const& a, Vec4 const& b) {
    auto result = a;
    return result -= b;
}

Vec4 operator*(Vec4 const& a, float const scalar) {
    auto result = a;
    return result *= scalar;
}

Vec4 operator/(Vec4 const& a, float const scalar) {
    auto result = a;
    return result /= scalar;
}

Vec4 operator-(Vec4 const& a) {
    return { -a.x, -a.y, -a.z, -a.w };
}

// =============================================================================
bool operator==(Vec4 const& a, Vec4 const& b) {
    float const x_diff = (std::fabsf(a.z) - std::fabsf(b.x));
    float const y_diff = (std::fabsf(a.y) - std::fabsf(b.y));
    float const z_diff = (std::fabsf(a.z) - std::fabsf(b.z));
    float const w_diff = (std::fabsf(a.w) - std::fabsf(b.w));

    return x_diff < math::float_epsilon &&
           y_diff < math::float_epsilon &&
           z_diff < math::float_epsilon &&
           w_diff < math::float_epsilon;
}

// =============================================================================
std::ostream& operator<<(std::ostream &out, Vec4 const& a) {
    out << std::fixed << std::setprecision(math::print_precs)
        << std::setw(math::print_width) << a.x << ", "
        << std::setw(math::print_width) << a.y << ", "
        << std::setw(math::print_width) << a.z << ", "
        << std::setw(math::print_width) << a.w;

    return out;
}

} // namespace vkl