#include "vklearnin/vklearnin.hpp"
#include "vklearnin/math/Vec3.hpp"

#include "vklearnin/math/math.hpp"

namespace vkl {
std::ostream& operator<<(std::ostream &out, Vec3 const& v) {
    out << std::fixed << std::setprecision(math::print_precs)
        << std::setw(math::print_width) << v.x << " "
        << std::setw(math::print_width) << v.y << " "
        << std::setw(math::print_width) << v.z;

    return out;
}

// =============================================================================
Vec3 Vec3::operator+(Vec3 const &other) const {
    return {
        x + other.x,
        y + other.y,
        z + other.z,
    };
}

Vec3 Vec3::operator-(Vec3 const &other) const {
    return {
        x - other.x,
        y - other.y,
        z - other.z,
    };
}

Vec3 & Vec3::operator+=(Vec3 const &other) {
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;
}

Vec3 & Vec3::operator-=(Vec3 const &other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;

    return *this;
}

Vec3 & Vec3::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;

    return *this;
}

Vec3 Vec3::operator-() const {
    return { -x, -y, -z  };
}

Vec3 & Vec3::operator-() {
    x = -x;
    y = -y;
    z = -z;

    return *this;
}

// =============================================================================
bool Vec3::operator==(const Vec3 &other) const {
    float const x_diff = std::abs(x - other.x);
    float const y_diff = std::abs(y - other.y);
    float const z_diff = std::abs(z - other.z);

    return (
        x_diff < math::float_epsilon &&
        y_diff < math::float_epsilon &&
        z_diff < math::float_epsilon
    );
}

// =============================================================================
Vec3 operator*(Vec3 const &v, float scalar) {
    return {
        v.x * scalar,
        v.y * scalar,
        v.z * scalar,
    };
}

Vec3 operator*(float scalar, Vec3 const &v) {
    return {
        v.x * scalar,
        v.y * scalar,
        v.z * scalar,
    };
}

// =============================================================================
Vec3::Vec3() :
    x { 0.0f },
    y { 0.0f },
    z { 0.0f }
{ }

Vec3::Vec3(float x, float y, float z) :
    x { x },
    y { y },
    z { z }
{ }

Vec3::Vec3(Vec4 const &v) :
    x { v.x },
    y { v.y },
    z { v.z }
{ }

} // namespace vkl