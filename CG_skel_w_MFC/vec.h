//////////////////////////////////////////////////////////////////////////////
//
//  --- vec.h ---
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
//  vec2.h - 2D vector
//
#pragma once
#include <cmath>
#include <iostream>
#include "GL/glew.h"
#define M_PI 3.14159265358979323846264338327
struct vec2 {

    GLfloat x, y;

    //
    //  --- Constructors and Destructors ---
    //

    vec2(GLfloat s = GLfloat(0.0)) noexcept : x(s), y(s) { }

    vec2(GLfloat x, GLfloat y) noexcept : x(x), y(y) { }

    vec2(const vec2& v) : x(v.x), y(v.y) { }

    //
    //  --- Indexing Operator ---
    //

    GLfloat& operator [](int i) noexcept { return *(&x + i); }
    const GLfloat operator [](int i) const noexcept { return *(&x + i); }

    //
    //  --- (non-modifying) Arithematic Operators ---
    //

     // unary minus operator
    vec2 operator -() const noexcept {
        return vec2(-x, -y);
    }

    vec2 operator +(const vec2& v) const noexcept {
        return vec2(x + v.x, y + v.y);
    }

    vec2 operator -(const vec2& v) const noexcept {
        return vec2(x - v.x, y - v.y);
    }

    vec2 operator *(const GLfloat s) const noexcept {
        return vec2(s * x, s * y);
    }

    vec2 operator *(const vec2& v) const noexcept {
        return vec2(x * v.x, y * v.y);
    }

    friend vec2 operator *(const GLfloat s, const vec2& v) noexcept {
        return v * s;
    }

    vec2 operator /(const GLfloat s) const noexcept {
        return *this * (GLfloat{ 1.0 } / s);
    }

    //
    //  --- (modifying) Arithematic Operators ---
    //

    vec2& operator +=(const vec2& v) noexcept {
        x += v.x; y += v.y; return *this;
    }

    vec2& operator -=(const vec2& v) noexcept {
        x -= v.x; y -= v.y; return *this;
    }

    vec2& operator *=(const GLfloat s) noexcept {
        x *= s; y *= s; return *this;
    }

    vec2& operator *=(const vec2& v) noexcept {
        x *= v.x; y *= v.y; return *this;
    }

    vec2& operator /=(const GLfloat s) noexcept {
        *this *= GLfloat{ 1.0 } / s;
        return *this;
    }

    //
    //  --- Insertion and Extraction Operators ---
    //

    friend std::ostream& operator <<(std::ostream& os, const vec2& v) {
        return os << "( " << v.x << ", " << v.y << " )";
    }

    friend std::istream& operator >>(std::istream& is, vec2& v) {
        return is >> v.x >> v.y;
    }

    //
    //  --- Conversion Operators ---
    //

    operator const GLfloat*() const noexcept { return &x; }

    operator GLfloat*() noexcept { return &x; }
};

//----------------------------------------------------------------------------
//
//  Non-class vec2 Methods
//

inline GLfloat dot(const vec2& u, const vec2& v) noexcept {
    return u.x * v.x + u.y * v.y;
}

inline GLfloat length(const vec2& v) noexcept {
    return std::sqrt(dot(v, v));
}

inline vec2 normalize(const vec2& v) noexcept {
    return v / length(v);
}

//////////////////////////////////////////////////////////////////////////////
//
//  vec3.h - 3D vector
//
//////////////////////////////////////////////////////////////////////////////

struct vec3 {

    GLfloat x, y, z;

    //
    //  --- Constructors and Destructors ---
    //

    vec3(GLfloat s = GLfloat(0.0)) noexcept : x(s), y(s), z(s) { }

    vec3(GLfloat x, GLfloat y, GLfloat z) noexcept : x(x), y(y), z(z) { }

    vec3(const vec3& v) : x(v.x), y(v.y), z(v.z) { }

    vec3(const vec2& v, const float f) noexcept : x(v.x), y(v.y), z(f) { }

    //
    //  --- Indexing Operator ---
    //

    GLfloat& operator [](int i) noexcept { return *(&x + i); }
    const GLfloat operator [](int i) const noexcept { return *(&x + i); }

    //
    //  --- (non-modifying) Arithematic Operators ---
    //

    // unary minus operator
    vec3 operator -() const noexcept {
        return vec3(-x, -y, -z);
    }

    vec3 operator +(const vec3& v) const noexcept {
        return vec3(x + v.x, y + v.y, z + v.z);
    }

    vec3 operator -(const vec3& v) const noexcept {
        return vec3(x - v.x, y - v.y, z - v.z);
    }

    vec3 operator *(const GLfloat s) const noexcept {
        return vec3(s * x, s * y, s * z);
    }

    vec3 operator *(const vec3& v) const noexcept {
        return vec3(x * v.x, y * v.y, z * v.z);
    }

    friend vec3 operator *(const GLfloat s, const vec3& v) noexcept {
        return v * s;
    }

    vec3 operator /(const GLfloat s) const noexcept {
        return *this * (GLfloat{ 1.0 } / s);
    }

    //
    //  --- (modifying) Arithematic Operators ---
    //

    vec3& operator +=(const vec3& v) noexcept {
        x += v.x;  y += v.y;  z += v.z;  return *this;
    }

    vec3& operator -=(const vec3& v) noexcept {
        x -= v.x;  y -= v.y;  z -= v.z;  return *this;
    }

    vec3& operator *=(const GLfloat s) noexcept {
        x *= s;  y *= s;  z *= s;  return *this;
    }

    vec3& operator *=(const vec3& v) noexcept {
        x *= v.x;  y *= v.y;  z *= v.z;  return *this;
    }

    vec3& operator /=(const GLfloat s) noexcept {
        *this *= GLfloat{ 1.0 } / s;
        return *this;
    }

    //
    //  --- Insertion and Extraction Operators ---
    //

    friend std::ostream& operator <<(std::ostream& os, const vec3& v) {
        return os << "( " << v.x << ", " << v.y << ", " << v.z << " )";
    }

    friend std::istream& operator >>(std::istream& is, vec3& v) {
        return is >> v.x >> v.y >> v.z;
    }

    //
    //  --- Conversion Operators ---
    //

    operator const GLfloat*() const noexcept { return &x; }

    operator GLfloat*() noexcept { return &x; }
};

//----------------------------------------------------------------------------
//
//  Non-class vec3 Methods
//

inline GLfloat dot(const vec3& u, const vec3& v) noexcept {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

inline GLfloat length(const vec3& v) noexcept {
    return std::sqrt(dot(v, v));
}

inline vec3 normalize(const vec3& v) noexcept {
    return v / length(v);
}

inline vec3 cross(const vec3& a, const vec3& b) noexcept {
    return vec3(a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x);
}

//////////////////////////////////////////////////////////////////////////////
//
//  vec4 - 4D vector
//
//////////////////////////////////////////////////////////////////////////////

struct vec4 {

    GLfloat x, y, z, w;

    //
    //  --- Constructors and Destructors ---
    //

    vec4(GLfloat s = GLfloat(0.0)) noexcept : x(s), y(s), z(s), w(s) { }

    vec4(GLfloat x, GLfloat y, GLfloat z, GLfloat w) noexcept : x(x), y(y), z(z), w(w) { }

    vec4(const vec4& v) : x(v.x), y(v.y), z(v.z), w(v.w) { }

    vec4(const vec3& v, const float w = 1.0) noexcept : x(v.x), y(v.y), z(v.z), w(w) { }

    vec4(const vec2& v, const float z, const float w) noexcept : x(v.x), y(v.y), z(z), w(w) { }

    //
    //  --- Indexing Operator ---
    //

    GLfloat& operator [](int i) noexcept { return *(&x + i); }
    const GLfloat operator [](int i) const noexcept { return *(&x + i); }

    //
    //  --- (non-modifying) Arithematic Operators ---
    //
    
    // unary minus operator
    vec4 operator -() const noexcept {
        return vec4(-x, -y, -z, -w);
    }

    vec4 operator +(const vec4& v) const noexcept {
        return vec4(x + v.x, y + v.y, z + v.z, w + v.w);
    }

    vec4 operator -(const vec4& v) const noexcept {
        return vec4(x - v.x, y - v.y, z - v.z, w - v.w);
    }

    vec4 operator *(const GLfloat s) const noexcept {
        return vec4(s * x, s * y, s * z, s * w);
    }

    vec4 operator *(const vec4& v) const noexcept {
        return vec4(x * v.x, y * v.y, z * v.z, w * v.w);
    }

    friend vec4 operator *(const GLfloat s, const vec4& v) noexcept {
        return v * s;
    }

    vec4 operator /(const GLfloat s) const noexcept {
        return *this * (GLfloat{ 1.0 } / s);
    }

    //
    //  --- (modifying) Arithematic Operators ---
    //

    vec4& operator +=(const vec4& v) noexcept {
        x += v.x;  y += v.y;  z += v.z;  w += v.w;  return *this;
    }

    vec4& operator -=(const vec4& v) noexcept {
        x -= v.x;  y -= v.y;  z -= v.z;  w -= v.w;  return *this;
    }

    vec4& operator *=(const GLfloat s) noexcept {
        x *= s;  y *= s;  z *= s;  w *= s;  return *this;
    }

    vec4& operator *=(const vec4& v) noexcept {
        x *= v.x, y *= v.y, z *= v.z, w *= v.w;  return *this;
    }

    vec4& operator /=(const GLfloat s) noexcept {
        *this *= GLfloat{ 1.0 } / s;
        return *this;
    }

    //
    //  --- Insertion and Extraction Operators ---
    //

    friend std::ostream& operator <<(std::ostream& os, const vec4& v) {
        return os << "( " << v.x << ", " << v.y << ", " << v.z << ", " << v.w << " )";
    }

    friend std::istream& operator >>(std::istream& is, vec4& v) {
        return is >> v.x >> v.y >> v.z >> v.w;
    }

    //
    //  --- Conversion Operators ---
    //

    operator const GLfloat*() const noexcept { return &x; }

    operator GLfloat*() noexcept { return &x; }
};

//----------------------------------------------------------------------------
//
//  Non-class vec4 Methods
//

inline GLfloat dot(const vec4& u, const vec4& v) noexcept {
    return u.x * v.x + u.y * v.y + u.z * v.z + u.w * v.w;
}

inline GLfloat length(const vec4& v) noexcept {
    return std::sqrt(dot(v, v));
}

inline vec4 normalize(const vec4& v) noexcept {
    return v / length(v);
}

inline vec4 cross(const vec4& a, const vec4& b) noexcept {
    return vec4(a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x,
                0); // cross products are intended for vectors anyway, not points
}

//----------------------------------------------------------------------------

// if i was really bored i'd rewrite these so vec4 : vec3 and vec3 : vec2... probably not necessary though lol
// maybe this has better performance actually idk
