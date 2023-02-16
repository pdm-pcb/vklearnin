#include "vklearnin/vklearnin.hpp"
#include "vklearnin/math/Mat4.hpp"

#include "vklearnin/math/math.hpp"

namespace vkl {

#ifdef VKL_USE_GLM
#else

std::ostream& operator<<(std::ostream &out, Mat4 const& m) {
    out << m.x << "\n" << m.y << "\n" << m.z << "\n" << m.w;
    return out;
}

// =============================================================================
Mat4 & Mat4::operator*=(Mat4 const &other) {
    float const x0 = (x.x * other.x.x) + (x.y * other.y.x) +
                     (x.z * other.z.x) + (x.w * other.w.x);
    float const y0 = (x.x * other.x.y) + (x.y * other.y.y) +
                     (x.z * other.z.y) + (x.w * other.w.y);
    float const z0 = (x.x * other.x.z) + (x.y * other.y.z) +
                     (x.z * other.z.z) + (x.w * other.w.z);
    float const w0 = (x.x * other.x.w) + (x.y * other.y.w) +
                     (x.z * other.z.w) + (x.w * other.w.w);

    float const x1 = (y.x * other.x.x) + (y.y * other.y.x) +
                     (y.z * other.z.x) + (y.w * other.w.x);
    float const y1 = (y.x * other.x.y) + (y.y * other.y.y) +
                     (y.z * other.z.y) + (y.w * other.w.y);
    float const z1 = (y.x * other.x.z) + (y.y * other.y.z) +
                     (y.z * other.z.z) + (y.w * other.w.z);
    float const w1 = (y.x * other.x.w) + (y.y * other.y.w) +
                     (y.z * other.z.w) + (y.w * other.w.w);

    float const x2 = (z.x * other.x.x) + (z.y * other.y.x) +
                     (z.z * other.z.x) + (z.w * other.w.x);
    float const y2 = (z.x * other.x.y) + (z.y * other.y.y) +
                     (z.z * other.z.y) + (z.w * other.w.y);
    float const z2 = (z.x * other.x.z) + (z.y * other.y.z) +
                     (z.z * other.z.z) + (z.w * other.w.z);
    float const w2 = (z.x * other.x.w) + (z.y * other.y.w) +
                     (z.z * other.z.w) + (z.w * other.w.w);

    float const x3 = (w.x * other.x.x) + (w.y * other.y.x) +
                     (w.z * other.z.x) + (w.w * other.w.x);
    float const y3 = (w.x * other.x.y) + (w.y * other.y.y) +
                     (w.z * other.z.y) + (w.w * other.w.y);
    float const z3 = (w.x * other.x.z) + (w.y * other.y.z) +
                     (w.z * other.z.z) + (w.w * other.w.z);
    float const w3 = (w.x * other.x.w) + (w.y * other.y.w) +
                     (w.z * other.z.w) + (w.w * other.w.w);

    x = { x0, y0, z0, w0 };
    y = { x1, y1, z1, w1 };
    z = { x2, y2, z2, w2 };
    w = { x3, y3, z3, w3 };

    return *this;
}

Mat4 Mat4::operator*(Mat4 const &other) const {
    auto result = *this;
    result *= other;
    return result;
}
    
Vec4 Mat4::operator*(Vec4 const &v) const {
    return {
        (x.x * v.x) + (x.y * v.y) + (x.z * v.z) + (x.w * v.w),
        (y.x * v.x) + (y.y * v.y) + (y.z * v.z) + (y.w * v.w),
        (z.x * v.x) + (z.y * v.y) + (z.z * v.z) + (z.w * v.w),
        (w.x * v.x) + (w.y * v.y) + (w.z * v.z) + (w.w * v.w),
    };
}

// =============================================================================
bool Mat4::operator==(Mat4 const &other) const {
    return (
        x == other.x &&
        y == other.y &&
        z == other.z &&
        w == other.w
    );
}

#endif // VKL_USE_GLM

} // namespace vkl