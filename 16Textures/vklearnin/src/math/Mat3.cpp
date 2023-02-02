#include "vklearnin/vklearnin.hpp"
#include "vklearnin/math/Mat3.hpp"

#include "vklearnin/math/math.hpp"

namespace vkl {

// =============================================================================
Mat3::Mat3(std::array<Vec3, 3> const& vecs) :
    rows { vecs }
{ }

Mat3::Mat3(std::array<Vec4, 3> const& vecs) :
    rows {{
        { vecs[0].x, vecs[0].y, vecs[0].z },
        { vecs[1].x, vecs[1].y, vecs[1].z },
        { vecs[2].x, vecs[2].y, vecs[2].z },
    }}
{ }

Mat3::Mat3(Vec3 const& x, Vec3 const& y, Vec3 const& z) :
    rows { x, y, z }
{ }

Mat3::Mat3(Vec4 const& x, Vec4 const&  y, Vec4 const& z) :
    rows {{
        { x.x, x.y, x.z },
        { y.x, y.y, y.z },
        { z.x, z.y, z.z },
    }}
{ }

Mat3::Mat3() :
    rows {
        Vec3::unit_x,
        Vec3::unit_y,
        Vec3::unit_z
    }
{ }

// =============================================================================
Mat3& Mat3::operator*=(Mat3 const& other) {
    float const x0 = (rows[0].x * other.rows[0].x) +
                     (rows[0].y * other.rows[1].x) +
                     (rows[0].z * other.rows[2].x);
    float const y0 = (rows[0].x * other.rows[0].y) +
                     (rows[0].y * other.rows[1].y) +
                     (rows[0].z * other.rows[2].y);
    float const z0 = (rows[0].x * other.rows[0].z) +
                     (rows[0].y * other.rows[1].z) +
                     (rows[0].z * other.rows[2].z);

    float const x1 = (rows[1].x * other.rows[0].x) +
                     (rows[1].y * other.rows[1].x) +
                     (rows[1].z * other.rows[2].x);
    float const y1 = (rows[1].x * other.rows[0].y) +
                     (rows[1].y * other.rows[1].y) +
                     (rows[1].z * other.rows[2].y);
    float const z1 = (rows[1].x * other.rows[0].z) +
                     (rows[1].y * other.rows[1].z) +
                     (rows[1].z * other.rows[2].z);

    float const x2 = (rows[2].x * other.rows[0].x) +
                     (rows[2].y * other.rows[1].x) +
                     (rows[2].z * other.rows[2].x);
    float const y2 = (rows[2].x * other.rows[0].y) +
                     (rows[2].y * other.rows[1].y) +
                     (rows[2].z * other.rows[2].y);
    float const z2 = (rows[2].x * other.rows[0].z) +
                     (rows[2].y * other.rows[1].z) +
                     (rows[2].z * other.rows[2].z);

    rows = {{
        { x0, y0, z0 },
        { x1, y1, z1 },
        { x2, y2, z2 },
    }};

    return *this;
}

// =============================================================================
Mat3 operator*(Mat3 const& a, Mat3 const& b) {
    Mat3 result = a;
    result *= b;

    return result;
}

// =============================================================================
bool operator==(Mat3 const& a, Mat3 const& b) {
    return a.rows[0] == b.rows[0] &&
           a.rows[1] == b.rows[1] &&
           a.rows[2] == b.rows[2];
}

// =============================================================================
std::ostream& operator<<(std::ostream& out, Mat3 const& a) {
    out << "[ " << a.rows[0] << " ]\n"
        << "[ " << a.rows[1] << " ]\n"
        << "[ " << a.rows[2] << " ]";

    return out;
}

} // namespace vkl