#include "vklearnin/vklearnin.hpp"
#include "vklearnin/math/mat4.hpp"

#include "vklearnin/math/math.hpp"

namespace vkl {

// =============================================================================
mat4::mat4(const std::array<vec4, 4> &vecs) :
    rows { vecs }
{ }

mat4::mat4(const vec4 &x, const vec4 &y, const vec4 &z, const vec4 &w) :
    rows { x, y, z, w }
{ }

mat4::mat4() :
    rows { vec4_unit_x, vec4_unit_y, vec4_unit_z, vec4_origin }
{ }

// =============================================================================
bool mat4::operator==(const mat4 &other) const {
    return rows[0] == other.rows[0] &&
           rows[1] == other.rows[1] &&
           rows[2] == other.rows[2] &&
           rows[3] == other.rows[3];
}

// =============================================================================
std::ostream & operator<<(std::ostream& out, const mat4& a) {
    out << "[ " << a.rows[0] << " ]\n"
        << "[ " << a.rows[1] << " ]\n"
        << "[ " << a.rows[2] << " ]\n"
        << "[ " << a.rows[3] << " ]";

    return out;
}

} // namespace vkl