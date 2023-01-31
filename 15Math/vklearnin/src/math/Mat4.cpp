#include "vklearnin/vklearnin.hpp"
#include "vklearnin/math/Mat4.hpp"

#include "vklearnin/math/math.hpp"

namespace vkl {

// =============================================================================
Mat4::Mat4(const std::array<Vec4, 4> &vecs) :
    rows { vecs }
{ }

Mat4::Mat4(const Vec4 &x, const Vec4 &y, const Vec4 &z, const Vec4 &w) :
    rows { x, y, z, w }
{ }

Mat4::Mat4() :
    rows {
        Vec4::unit_x,
        Vec4::unit_y,
        Vec4::unit_z,
        Vec4::origin
    }
{ }

// =============================================================================
Mat4& Mat4::operator=(const Mat3 &other) {
    rows = {{
        { other.rows[0].x, other.rows[0].y, other.rows[0].z, 0.0f },
        { other.rows[1].x, other.rows[1].y, other.rows[1].z, 0.0f },
        { other.rows[2].x, other.rows[2].y, other.rows[2].z, 0.0f },
        { 0.0f, 0.0f, 0.0f, 1.0f },
    }};

    return *this;
}

// =============================================================================
Mat4& Mat4::operator*=(const Mat4 &other) {
    const float x0 = (rows[0].x * other.rows[0].x) +
                     (rows[0].y * other.rows[1].x) +
                     (rows[0].z * other.rows[2].x) +
                     (rows[0].w * other.rows[3].x);
    const float y0 = (rows[0].x * other.rows[0].y) +
                     (rows[0].y * other.rows[1].y) +
                     (rows[0].z * other.rows[2].y) +
                     (rows[0].w * other.rows[3].y);
    const float z0 = (rows[0].x * other.rows[0].z) +
                     (rows[0].y * other.rows[1].z) +
                     (rows[0].z * other.rows[2].z) +
                     (rows[0].w * other.rows[3].z);
    const float w0 = (rows[0].x * other.rows[0].w) +
                     (rows[0].y * other.rows[1].w) +
                     (rows[0].z * other.rows[2].w) +
                     (rows[0].w * other.rows[3].w);

    const float x1 = (rows[1].x * other.rows[0].x) +
                     (rows[1].y * other.rows[1].x) +
                     (rows[1].z * other.rows[2].x) +
                     (rows[1].w * other.rows[3].x);
    const float y1 = (rows[1].x * other.rows[0].y) +
                     (rows[1].y * other.rows[1].y) +
                     (rows[1].z * other.rows[2].y) +
                     (rows[1].w * other.rows[3].y);
    const float z1 = (rows[1].x * other.rows[0].z) +
                     (rows[1].y * other.rows[1].z) +
                     (rows[1].z * other.rows[2].z) +
                     (rows[1].w * other.rows[3].z);
    const float w1 = (rows[1].x * other.rows[0].w) +
                     (rows[1].y * other.rows[1].w) +
                     (rows[1].z * other.rows[2].w) +
                     (rows[1].w * other.rows[3].w);

    const float x2 = (rows[2].x * other.rows[0].x) +
                     (rows[2].y * other.rows[1].x) +
                     (rows[2].z * other.rows[2].x) +
                     (rows[2].w * other.rows[3].x);
    const float y2 = (rows[2].x * other.rows[0].y) +
                     (rows[2].y * other.rows[1].y) +
                     (rows[2].z * other.rows[2].y) +
                     (rows[2].w * other.rows[3].y);
    const float z2 = (rows[2].x * other.rows[0].z) +
                     (rows[2].y * other.rows[1].z) +
                     (rows[2].z * other.rows[2].z) +
                     (rows[2].w * other.rows[3].z);
    const float w2 = (rows[2].x * other.rows[0].w) +
                     (rows[2].y * other.rows[1].w) +
                     (rows[2].z * other.rows[2].w) +
                     (rows[2].w * other.rows[3].w);

    const float x3 = (rows[3].x * other.rows[0].x) +
                     (rows[3].y * other.rows[1].x) +
                     (rows[3].z * other.rows[2].x) +
                     (rows[3].w * other.rows[3].x);
    const float y3 = (rows[3].x * other.rows[0].y) +
                     (rows[3].y * other.rows[1].y) +
                     (rows[3].z * other.rows[2].y) +
                     (rows[3].w * other.rows[3].y);
    const float z3 = (rows[3].x * other.rows[0].z) +
                     (rows[3].y * other.rows[1].z) +
                     (rows[3].z * other.rows[2].z) +
                     (rows[3].w * other.rows[3].z);
    const float w3 = (rows[3].x * other.rows[0].w) +
                     (rows[3].y * other.rows[1].w) +
                     (rows[3].z * other.rows[2].w) +
                     (rows[3].w * other.rows[3].w);

    rows = {{
        { x0, y0, z0, w0 },
        { x1, y1, z1, w1 },
        { x2, y2, z2, w2 },
        { x3, y3, z3, w3 },
    }};

    return *this;
}

// =============================================================================
Mat4 operator*(const Mat4 &a, const Mat4 &b) {
    Mat4 result = a;
    result *= b;

    return result;
}

// =============================================================================
bool operator==(const Mat4 &a, const Mat4 &b) {
    return a.rows[0] == b.rows[0] &&
           a.rows[1] == b.rows[1] &&
           a.rows[2] == b.rows[2] &&
           a.rows[3] == b.rows[3];
}

// =============================================================================
std::ostream& operator<<(std::ostream& out, const Mat4& a) {
    out << "[ " << a.rows[0] << " ]\n"
        << "[ " << a.rows[1] << " ]\n"
        << "[ " << a.rows[2] << " ]\n"
        << "[ " << a.rows[3] << " ]";

    return out;
}

} // namespace vkl