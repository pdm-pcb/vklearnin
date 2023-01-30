#include "vklearnin/vklearnin.hpp"
#include "vklearnin/math/Mat3.hpp"

#include "vklearnin/math/math.hpp"

namespace vkl {

// =============================================================================
Mat3::Mat3(const std::array<Vec3, 3> &vecs) :
    rows { vecs }
{ }

Mat3::Mat3(const std::array<Vec4, 3> &vecs) :
    rows {{
        { vecs[0].x, vecs[0].y, vecs[0].z },
        { vecs[1].x, vecs[1].y, vecs[1].z },
        { vecs[2].x, vecs[2].y, vecs[2].z },
    }}
{ }

Mat3::Mat3(const Vec3 &x, const Vec3 &y, const Vec3 &z) :
    rows { x, y, z }
{ }

Mat3::Mat3(const Vec4 &x, const Vec4 &y, const Vec4 &z) :
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

#ifdef VKL_USE_GLM
    Mat3::Mat3(const glm::mat3 &other) {
        rows = {{
            Vec3(other[0]),
            Vec3(other[1]),
            Vec3(other[2])
        }};
    }

    Mat3& Mat3::operator=(const glm::mat3 &other) {
        rows = {{
            Vec3(other[0]),
            Vec3(other[1]),
            Vec3(other[2])
        }};

        return *this;
    }
#endif // VKL_USE_GLM

// =============================================================================
Mat3& Mat3::operator*=(const Mat3 &other) {
    const float x0 = (rows[0].x * other.rows[0].x) +
                     (rows[0].y * other.rows[1].x) +
                     (rows[0].z * other.rows[2].x);
    const float y0 = (rows[0].x * other.rows[0].y) +
                     (rows[0].y * other.rows[1].y) +
                     (rows[0].z * other.rows[2].y);
    const float z0 = (rows[0].x * other.rows[0].z) +
                     (rows[0].y * other.rows[1].z) +
                     (rows[0].z * other.rows[2].z);

    const float x1 = (rows[1].x * other.rows[0].x) +
                     (rows[1].y * other.rows[1].x) +
                     (rows[1].z * other.rows[2].x);
    const float y1 = (rows[1].x * other.rows[0].y) +
                     (rows[1].y * other.rows[1].y) +
                     (rows[1].z * other.rows[2].y);
    const float z1 = (rows[1].x * other.rows[0].z) +
                     (rows[1].y * other.rows[1].z) +
                     (rows[1].z * other.rows[2].z);

    const float x2 = (rows[2].x * other.rows[0].x) +
                     (rows[2].y * other.rows[1].x) +
                     (rows[2].z * other.rows[2].x);
    const float y2 = (rows[2].x * other.rows[0].y) +
                     (rows[2].y * other.rows[1].y) +
                     (rows[2].z * other.rows[2].y);
    const float z2 = (rows[2].x * other.rows[0].z) +
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
Mat3 operator*(const Mat3 &a, const Mat3 &b) {
    Mat3 result = a;
    result *= b;

    return result;
}

// =============================================================================
bool operator==(const Mat3 &a, const Mat3 &b) {
    return a.rows[0] == b.rows[0] &&
           a.rows[1] == b.rows[1] &&
           a.rows[2] == b.rows[2];
}

// =============================================================================
std::ostream& operator<<(std::ostream& out, const Mat3& a) {
    out << "[ " << a.rows[0] << " ]\n"
        << "[ " << a.rows[1] << " ]\n"
        << "[ " << a.rows[2] << " ]";

    return out;
}

} // namespace vkl