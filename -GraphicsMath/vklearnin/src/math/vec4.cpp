#include "vklearnin/vklearnin.hpp"
#include "vklearnin/math/vec4.hpp"

#include "vklearnin/math/math.hpp"

namespace vkl {

// =============================================================================
vec4::vec4(const std::array<float, 4> &vec) :
    x { vec[0] },
    y { vec[1] },
    z { vec[2] },
    w { vec[3] },
    length { calc_length(*this) }
{ }

vec4::vec4(const float x, const float y, const float z, const float w) :
    x { x },
    y { y },
    z { z },
    w { w },
    length { calc_length(*this) }
{ }

vec4::vec4() :
    x { 0.0f },
    y { 0.0f },
    z { 0.0f },
    w { 0.0f },
    length { 0.0f }
{ }

// =============================================================================
vec4 vec4::operator+(const vec4 &other) const {
    return { x + other.x, y + other.y, z + other.z, w + other.w };
}

vec4 vec4::operator-(const vec4 &other) const {
    return { x - other.x, y - other.y, z - other.z, w - other.w };
}

vec4 vec4::operator*(const float scalar) const {
    return { x * scalar, y * scalar, z * scalar, w };
}

vec4 vec4::operator/(const float scalar) const {
    return { x / scalar, y / scalar, z / scalar, w };
}

// =============================================================================
vec4 & vec4::operator+=(const vec4 &other) {
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;

    return *this;
}

vec4 & vec4::operator-=(const vec4 &other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;

    return *this;
}

vec4 & vec4::operator*=(const float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;

    return *this;
}

vec4 & vec4::operator/=(const float scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;

    return *this;
}

// =============================================================================
bool vec4::operator==(const vec4 &other) const {
    const float x_diff = (std::fabsf(x) - std::fabsf(other.x));
    const float y_diff = (std::fabsf(y) - std::fabsf(other.y));
    const float z_diff = (std::fabsf(z) - std::fabsf(other.z));
    const float w_diff = (std::fabsf(w) - std::fabsf(other.w));

    return x_diff < math::float_epsilon &&
           y_diff < math::float_epsilon &&
           z_diff < math::float_epsilon &&
           w_diff < math::float_epsilon;
}

// =============================================================================
std::ostream & operator<<(std::ostream &out, const vec4 &a) {
    out << std::fixed << std::setprecision(print_precs)
        << std::setw(print_width) << a.x << ", "
        << std::setw(print_width) << a.y << ", "
        << std::setw(print_width) << a.z << ", "
        << std::setw(print_width) << a.w;

    return out;
}

} // namespace vkl