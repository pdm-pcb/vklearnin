#include "vklearnin/vklearnin.hpp"
#include "vklearnin/math/Vec3.hpp"

#include "vklearnin/math/math.hpp"

namespace vkl {

// =============================================================================
Vec3::Vec3(const std::array<float, 4> &vec) :
    x { vec[0] },
    y { vec[1] },
    z { vec[2] }
{ }

Vec3::Vec3(const float x, const float y, const float z) :
    x { x },
    y { y },
    z { z }
{ }

Vec3::Vec3() :
    x { 0.0f },
    y { 0.0f },
    z { 0.0f }
{ }

Vec3::Vec3(const Vec4 &other) :
    x { other.x },
    y { other.y },
    z { other.z }
{ }

Vec3& Vec3::operator=(const Vec4 &other) {
    x = other.x;
    y = other.y;
    z = other.z;

    return *this;
}

#ifdef VKL_USE_GLM
    Vec3::Vec3(const glm::vec3 &other) :
        x { other.x },
        y { other.y },
        z { other.z }
    { }

    Vec3& Vec3::operator=(const glm::vec3 &other) {
        x = other.x;
        y = other.y;
        z = other.z;

        return *this;
    }
#endif // VKL_USE_GLM

// =============================================================================
Vec3& Vec3::operator+=(const Vec3 &other) {
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;
}

Vec3& Vec3::operator-=(const Vec3 &other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;

    return *this;
}

Vec3& Vec3::operator*=(const float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;

    return *this;
}

Vec3& Vec3::operator/=(const float scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;

    return *this;
}

// =============================================================================
Vec3 operator+(const Vec3 &a, const Vec3 &b) {
    auto result = a;
    return result += b;
}

Vec3 operator-(const Vec3 &a, const Vec3 &b) {
    auto result = a;
    return result -= b;
}

Vec3 operator*(const Vec3 &a, const float scalar) {
    auto result = a;
    return result *= scalar;
}

Vec3 operator/(const Vec3 &a, const float scalar) {
    auto result = a;
    return result /= scalar;
}

Vec3 operator-(const Vec3 &a) {
    return { -a.x, -a.y, -a.z };
}

// =============================================================================
bool operator==(const Vec3 &a, const Vec3 &b) {
    const float x_diff = (std::fabsf(a.z) - std::fabsf(b.x));
    const float y_diff = (std::fabsf(a.y) - std::fabsf(b.y));
    const float z_diff = (std::fabsf(a.z) - std::fabsf(b.z));

    return x_diff < math::float_epsilon &&
           y_diff < math::float_epsilon &&
           z_diff < math::float_epsilon;
}

// =============================================================================
std::ostream& operator<<(std::ostream &out, const Vec3 &a) {
    out << std::fixed << std::setprecision(math::print_precs)
        << std::setw(math::print_width) << a.x << ", "
        << std::setw(math::print_width) << a.y << ", "
        << std::setw(math::print_width) << a.z;

    return out;
}

} // namespace vkl