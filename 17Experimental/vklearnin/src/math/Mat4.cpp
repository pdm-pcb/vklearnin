#include "vklearnin/vklearnin.hpp"
#include "vklearnin/math/Mat4.hpp"

#include "vklearnin/math/math.hpp"

namespace vkl {

// =============================================================================
Mat4::Mat4(std::array<Vec4, 4> const& vecs) :
    rows { vecs }
{ }

Mat4::Mat4(Vec4 const& x, Vec4 const& y, Vec4 const& z, Vec4 const&  w) :
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
Mat4& Mat4::operator=(Mat3 const& other) {
    rows = {{
        { other.rows[0].x, other.rows[0].y, other.rows[0].z, 0.0f },
        { other.rows[1].x, other.rows[1].y, other.rows[1].z, 0.0f },
        { other.rows[2].x, other.rows[2].y, other.rows[2].z, 0.0f },
        { 0.0f, 0.0f, 0.0f, 1.0f },
    }};

    return *this;
}

// =============================================================================
Mat4& Mat4::operator*=(Mat4 const& other) {
    float const x0 = (rows[0].x * other.rows[0].x) +
                     (rows[0].y * other.rows[1].x) +
                     (rows[0].z * other.rows[2].x) +
                     (rows[0].w * other.rows[3].x);
    float const y0 = (rows[0].x * other.rows[0].y) +
                     (rows[0].y * other.rows[1].y) +
                     (rows[0].z * other.rows[2].y) +
                     (rows[0].w * other.rows[3].y);
    float const z0 = (rows[0].x * other.rows[0].z) +
                     (rows[0].y * other.rows[1].z) +
                     (rows[0].z * other.rows[2].z) +
                     (rows[0].w * other.rows[3].z);
    float const w0 = (rows[0].x * other.rows[0].w) +
                     (rows[0].y * other.rows[1].w) +
                     (rows[0].z * other.rows[2].w) +
                     (rows[0].w * other.rows[3].w);

    float const x1 = (rows[1].x * other.rows[0].x) +
                     (rows[1].y * other.rows[1].x) +
                     (rows[1].z * other.rows[2].x) +
                     (rows[1].w * other.rows[3].x);
    float const y1 = (rows[1].x * other.rows[0].y) +
                     (rows[1].y * other.rows[1].y) +
                     (rows[1].z * other.rows[2].y) +
                     (rows[1].w * other.rows[3].y);
    float const z1 = (rows[1].x * other.rows[0].z) +
                     (rows[1].y * other.rows[1].z) +
                     (rows[1].z * other.rows[2].z) +
                     (rows[1].w * other.rows[3].z);
    float const w1 = (rows[1].x * other.rows[0].w) +
                     (rows[1].y * other.rows[1].w) +
                     (rows[1].z * other.rows[2].w) +
                     (rows[1].w * other.rows[3].w);

    float const x2 = (rows[2].x * other.rows[0].x) +
                     (rows[2].y * other.rows[1].x) +
                     (rows[2].z * other.rows[2].x) +
                     (rows[2].w * other.rows[3].x);
    float const y2 = (rows[2].x * other.rows[0].y) +
                     (rows[2].y * other.rows[1].y) +
                     (rows[2].z * other.rows[2].y) +
                     (rows[2].w * other.rows[3].y);
    float const z2 = (rows[2].x * other.rows[0].z) +
                     (rows[2].y * other.rows[1].z) +
                     (rows[2].z * other.rows[2].z) +
                     (rows[2].w * other.rows[3].z);
    float const w2 = (rows[2].x * other.rows[0].w) +
                     (rows[2].y * other.rows[1].w) +
                     (rows[2].z * other.rows[2].w) +
                     (rows[2].w * other.rows[3].w);

    float const x3 = (rows[3].x * other.rows[0].x) +
                     (rows[3].y * other.rows[1].x) +
                     (rows[3].z * other.rows[2].x) +
                     (rows[3].w * other.rows[3].x);
    float const y3 = (rows[3].x * other.rows[0].y) +
                     (rows[3].y * other.rows[1].y) +
                     (rows[3].z * other.rows[2].y) +
                     (rows[3].w * other.rows[3].y);
    float const z3 = (rows[3].x * other.rows[0].z) +
                     (rows[3].y * other.rows[1].z) +
                     (rows[3].z * other.rows[2].z) +
                     (rows[3].w * other.rows[3].z);
    float const w3 = (rows[3].x * other.rows[0].w) +
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
Mat4 operator*(Mat4 const& a, Mat4 const& b) {
    Mat4 result = a;
    result *= b;

    return result;
}

// =============================================================================
bool operator==(Mat4 const& a, Mat4 const& b) {
    return a.rows[0] == b.rows[0] &&
           a.rows[1] == b.rows[1] &&
           a.rows[2] == b.rows[2] &&
           a.rows[3] == b.rows[3];
}

// =============================================================================
std::ostream& operator<<(std::ostream& out, Mat4 const& a) {
    out << "[ " << a.rows[0] << " ]\n"
        << "[ " << a.rows[1] << " ]\n"
        << "[ " << a.rows[2] << " ]\n"
        << "[ " << a.rows[3] << " ]";

    return out;
}

} // namespace vkl