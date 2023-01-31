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

Vec4::Vec4(const float x, const float y, const float z, const float w) :
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

Vec4::Vec4(const Vec3 &other, const float w) :
    x { other.x },
    y { other.y },
    z { other.z },
    w { w }
{ }

// =============================================================================
Vec4& Vec4::operator+=(const Vec4 &other) {
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;

    return *this;
}

Vec4& Vec4::operator-=(const Vec4 &other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;

    return *this;
}

Vec4& Vec4::operator*=(const float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;

    return *this;
}

Vec4& Vec4::operator/=(const float scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;

    return *this;
}

// =============================================================================
Vec4 operator+(const Vec4 &a, const Vec4 &b) {
    auto result = a;
    return result += b;
}

Vec4 operator-(const Vec4 &a, const Vec4 &b) {
    auto result = a;
    return result -= b;
}

Vec4 operator*(const Vec4 &a, const float scalar) {
    auto result = a;
    return result *= scalar;
}

Vec4 operator/(const Vec4 &a, const float scalar) {
    auto result = a;
    return result /= scalar;
}

Vec4 operator-(const Vec4 &a) {
    return { -a.x, -a.y, -a.z, -a.w };
}

// =============================================================================
bool operator==(const Vec4 &a, const Vec4 &b) {
    const float x_diff = (std::fabsf(a.z) - std::fabsf(b.x));
    const float y_diff = (std::fabsf(a.y) - std::fabsf(b.y));
    const float z_diff = (std::fabsf(a.z) - std::fabsf(b.z));
    const float w_diff = (std::fabsf(a.w) - std::fabsf(b.w));

    return x_diff < math::float_epsilon &&
           y_diff < math::float_epsilon &&
           z_diff < math::float_epsilon &&
           w_diff < math::float_epsilon;
}

// =============================================================================
std::ostream& operator<<(std::ostream &out, const Vec4 &a) {
    out << std::fixed << std::setprecision(math::print_precs)
        << std::setw(math::print_width) << a.x << ", "
        << std::setw(math::print_width) << a.y << ", "
        << std::setw(math::print_width) << a.z << ", "
        << std::setw(math::print_width) << a.w;

    return out;
}

} // namespace vkl