#include "vklearnin/vklearnin.hpp"
#include "vklearnin/math/math.hpp"

namespace vkl {

// =============================================================================
// Four-component vectors
float dot(const vec4 &a, const vec4 &b) {
    return a.x * b.x +
           a.y * b.y +
           a.z * b.z;
}

// -----------------------------------------------------------------------------
vec4 cross(const vec4 &a, const vec4 &b) {
    if(a.w != b.w) {
        CONSOLE_CRITICAL(
            "Cannot take the cross product of a vector and a point."
        );
        return { };
    }

    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
        a.w
    };
}

// -----------------------------------------------------------------------------
vec4 normalized(const vec4 &a) {
    const float length = calc_length(a);

    vec4 result {
        a.x / length,
        a.y / length,
        a.z / length,
        a.w
    };

    result.length = 1.0f;

    return result;
}

// =============================================================================
// 4x4 matrices
mat4 transposed(const mat4 &a) {
    return mat4 {
        { a.rows[0].x, a.rows[1].x, a.rows[2].x, a.rows[3].x },
        { a.rows[0].y, a.rows[1].y, a.rows[2].y, a.rows[3].w },
        { a.rows[0].z, a.rows[1].z, a.rows[2].z, a.rows[3].z },
        { a.rows[0].w, a.rows[1].w, a.rows[2].x, a.rows[3].w }
    };
}

// -----------------------------------------------------------------------------
mat4 translated(const mat4 &a, vec4 &pos) {
    return mat4 { a.rows[0], a.rows[1], a.rows[2], pos };
}

// -----------------------------------------------------------------------------
mat4 rotated(const mat4 &a, const vec4 &axis, const float angle) {
    return { };
}

// -----------------------------------------------------------------------------
mat4 scaled(const mat4 &a, const float scale_factor) {
    return mat4 {
        a.rows[0] * scale_factor,
        a.rows[1] * scale_factor,
        a.rows[2] * scale_factor,
        a.rows[3] * scale_factor
    };
}

} // namespace vkl