#include "vklearnin/vklearnin.hpp"
#include "vklearnin/math/Vec4.hpp"

#include "vklearnin/math/math.hpp"

namespace vkl {

// =============================================================================
void Vec4::normalize() {
    auto length = Vec4::length2();
    if(length <= 0.0f || length == 1.0f) {
        return;
    }

    float length_inv = 1.0f / std::sqrtf(length);

    x *= length_inv;
    y *= length_inv;
    z *= length_inv;
}

// =============================================================================
Vec4 Vec4::normalized() const {
    auto result(*this);
    result.normalize();
    return result;
}

// =============================================================================
Vec4 Vec4::operator+(Vec4 const &other) const {
    return { 
        x + other.x,
        y + other.y,
        z + other.z
    };
}

Vec4 Vec4::operator-(Vec4 const &other) const {
    return { 
        x - other.x,
        y - other.y,
        z - other.z
    };
}

Vec4 & Vec4::operator+=(Vec4 const &other) {
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;
}

Vec4 & Vec4::operator-=(Vec4 const &other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;

    return *this;
}

Vec4 Vec4::operator*(float scalar) const {
    return { 
        x * scalar,
        y * scalar,
        z * scalar
    };
}

Vec4 & Vec4::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;

    return *this;
}

Vec4 Vec4::operator-() const {
    return { -x, -y, -z };
}

Vec4 & Vec4::operator-() {
    x = -x;
    y = -y;
    z = -z;

    return *this;
}

// =============================================================================
bool Vec4::operator==(const Vec4 &other) const {
    float x_diff = std::fabsf(x - other.x);
    float y_diff = std::fabsf(y - other.y);
    float z_diff = std::fabsf(z - other.z);

    return (
        x_diff < math::float_epsilon &&
        y_diff < math::float_epsilon &&
        z_diff < math::float_epsilon &&
        w == w
    );
}

// =============================================================================
std::ostream& operator<<(std::ostream &out, Vec4 const& a) {
    out << std::fixed << std::setprecision(math::print_precs)
        << std::setw(math::print_width) << a.x << " "
        << std::setw(math::print_width) << a.y << " "
        << std::setw(math::print_width) << a.z << " "
        << std::setw(math::print_width) << a.w;

    return out;
}

} // namespace vkl