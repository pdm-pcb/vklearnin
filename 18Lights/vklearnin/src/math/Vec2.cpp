#include "vklearnin/vklearnin.hpp"
#include "vklearnin/math/Vec2.hpp"

#include "vklearnin/math/math.hpp"

namespace vkl {
std::ostream& operator<<(std::ostream &out, Vec2 const& v) {
    out << std::fixed << std::setprecision(math::print_precs)
        << std::setw(math::print_width) << v.x << " "
        << std::setw(math::print_width) << v.y;

    return out;
}

// =============================================================================
Vec2 Vec2::operator+(Vec2 const &other) const {
    return {
        x + other.x,
        y + other.y,
    };
}

Vec2 Vec2::operator-(Vec2 const &other) const {
    return {
        x - other.x,
        y - other.y,
    };
}

Vec2 & Vec2::operator+=(Vec2 const &other) {
    x += other.x;
    y += other.y;

    return *this;
}

Vec2 & Vec2::operator-=(Vec2 const &other) {
    x -= other.x;
    y -= other.y;

    return *this;
}

Vec2 & Vec2::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;

    return *this;
}

Vec2 Vec2::operator-() const {
    return { -x, -y  };
}

Vec2 & Vec2::operator-() {
    x = -x;
    y = -y;

    return *this;
}

// =============================================================================
bool Vec2::operator==(const Vec2 &other) const {
    float const x_diff = std::abs(x - other.x);
    float const y_diff = std::abs(y - other.y);

    return (
        x_diff < math::float_epsilon &&
        y_diff < math::float_epsilon
    );
}

// =============================================================================
Vec2 operator*(Vec2 const &v, float scalar) {
    return {
        v.x * scalar,
        v.y * scalar,
    };
}

Vec2 operator*(float scalar, Vec2 const &v) {
    return {
        v.x * scalar,
        v.y * scalar,
    };
}

} // namespace vkl