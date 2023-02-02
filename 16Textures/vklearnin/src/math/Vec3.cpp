#include "vklearnin/vklearnin.hpp"
#include "vklearnin/math/Vec3.hpp"

#include "vklearnin/math/math.hpp"

namespace vkl {

// =============================================================================
Vec3::Vec3(std::array<float, 4> const& vec) :
    x { vec[0] },
    y { vec[1] },
    z { vec[2] }
{ }

Vec3::Vec3(float const x, float const y, float const z) :
    x { x },
    y { y },
    z { z }
{ }

Vec3::Vec3() :
    x { 0.0f },
    y { 0.0f },
    z { 0.0f }
{ }

Vec3::Vec3(Vec4 const& other) :
    x { other.x },
    y { other.y },
    z { other.z }
{ }

Vec3& Vec3::operator=(Vec4 const& other) {
    x = other.x;
    y = other.y;
    z = other.z;

    return *this;
}

// =============================================================================
Vec3& Vec3::operator+=(Vec3 const& other) {
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;
}

Vec3& Vec3::operator-=(Vec3 const& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;

    return *this;
}

Vec3& Vec3::operator*=(float const scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;

    return *this;
}

Vec3& Vec3::operator/=(float const scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;

    return *this;
}

// =============================================================================
Vec3 operator+(Vec3 const& a, Vec3 const& b) {
    auto result = a;
    return result += b;
}

Vec3 operator-(Vec3 const& a, Vec3 const& b) {
    auto result = a;
    return result -= b;
}

Vec3 operator*(Vec3 const& a, float const scalar) {
    auto result = a;
    return result *= scalar;
}

Vec3 operator/(Vec3 const& a, float const scalar) {
    auto result = a;
    return result /= scalar;
}

Vec3 operator-(Vec3 const& a) {
    return { -a.x, -a.y, -a.z };
}

// =============================================================================
bool operator==(Vec3 const& a, Vec3 const& b) {
    float const x_diff = (std::fabsf(a.z) - std::fabsf(b.x));
    float const y_diff = (std::fabsf(a.y) - std::fabsf(b.y));
    float const z_diff = (std::fabsf(a.z) - std::fabsf(b.z));

    return x_diff < math::float_epsilon &&
           y_diff < math::float_epsilon &&
           z_diff < math::float_epsilon;
}

// =============================================================================
std::ostream& operator<<(std::ostream &out, Vec3 const& a) {
    out << std::fixed << std::setprecision(math::print_precs)
        << std::setw(math::print_width) << a.x << ", "
        << std::setw(math::print_width) << a.y << ", "
        << std::setw(math::print_width) << a.z;

    return out;
}

} // namespace vkl