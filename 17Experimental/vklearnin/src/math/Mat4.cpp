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
    auto const Ax = x;
    auto const Ay = y;
    auto const Az = z;
    auto const Aw = w;

    auto const Bx = other.x;
    auto const By = other.y;
    auto const Bz = other.z;
    auto const Bw = other.w;

    x = (Ax * Bx.x) + (Ay * Bx.y) + (Az * Bx.z) + (Aw * Bx.w);
    y = (Ax * By.x) + (Ay * By.y) + (Az * By.z) + (Aw * By.w);
    z = (Ax * Bz.x) + (Ay * Bz.y) + (Az * Bz.z) + (Aw * Bz.w);
    w = (Ax * Bw.x) + (Ay * Bw.y) + (Az * Bw.z) + (Aw * Bw.w);

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