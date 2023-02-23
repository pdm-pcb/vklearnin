#include "vklearnin/vklearnin.hpp"
#include "vklearnin/math/Vec4.hpp"

#include "vklearnin/math/math.hpp"

namespace vkl {
#ifdef VKL_USE_GLM
#else
std::ostream& operator<<(std::ostream &out, Vec4 const& v) {
    out << std::fixed << std::setprecision(math::print_precs)
        << std::setw(math::print_width) << v.x << " "
        << std::setw(math::print_width) << v.y << " "
        << std::setw(math::print_width) << v.z << " "
        << std::setw(math::print_width) << v.w;

    return out;
}

// =============================================================================
Vec4 Vec4::operator+(Vec4 const &other) const {
    return { 
        x + other.x,
        y + other.y,
        z + other.z,
        w + other.w,
    };
}

Vec4 Vec4::operator-(Vec4 const &other) const {
    return { 
        x - other.x,
        y - other.y,
        z - other.z,
        w - other.w,
    };
}

Vec4 & Vec4::operator+=(Vec4 const &other) {
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;

    return *this;
}

Vec4 & Vec4::operator-=(Vec4 const &other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;

    return *this;
}

Vec4 & Vec4::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;

    return *this;
}

Vec4 Vec4::operator-() const {
    return { -x, -y, -z, -w };
}

Vec4 & Vec4::operator-() {
    x = -x;
    y = -y;
    z = -z;
    w = -w;

    return *this;
}

// =============================================================================
bool Vec4::operator==(const Vec4 &other) const {
    float x_diff = std::fabsf(x - other.x);
    float y_diff = std::fabsf(y - other.y);
    float z_diff = std::fabsf(z - other.z);
    float w_diff = std::fabsf(w - other.w);

    return (
        x_diff < math::float_epsilon &&
        y_diff < math::float_epsilon &&
        z_diff < math::float_epsilon &&
        w_diff < math::float_epsilon
    );
}

// =============================================================================
Vec4 operator*(Vec4 const &v, float scalar) {
    return { 
        v.x * scalar,
        v.y * scalar,
        v.z * scalar,
        v.w * scalar,
    };
}

Vec4 operator*(float scalar, Vec4 const &v) {
    return { 
        v.x * scalar,
        v.y * scalar,
        v.z * scalar,
        v.w * scalar,
    };
}

#endif // VKL_USE_GLM

} // namespace vkl