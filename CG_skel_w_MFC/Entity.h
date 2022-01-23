#pragma once

#include "vec.h"
#include "mat.h"

// An entity is anything that exists in our scene and can be moved - i.e. camera, model, light, etc
struct Entity
{
	mat4 self; // the self frame, so either a camera's view frame, or a model's model frame (duh)
	mat4 world; // the world frame
	mat4 getTransform() const { return world * self; }
};

// matrix getters/setters/manipulations

inline void printTransform(mat4& mat) noexcept {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%f ", mat[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

inline void setPosition(mat4& mat, const vec3& v) noexcept
{
	mat[0][3] = v.x;
	mat[1][3] = v.y;
	mat[2][3] = v.z;
}

inline vec3 getPosition(const mat4& mat)
{
	return vec3(mat[0][3], mat[1][3], mat[2][3]);
}

// other than position, i don't think we really care about getting/setting scale and rotation... besides hard resetting

inline void moveBy(mat4& mat, const vec4& v)
{
	mat = Translate(v) * mat;
}

inline void rotateBy(mat4& mat, const vec3& r, const vec3& origin) // r is in euler angles, measured in radians
{
	moveBy(mat, -origin);
	mat = RotateZ(r.z) * RotateY(r.y) * RotateX(r.x) * mat;
	moveBy(mat, origin);
}

inline void rotateBy(mat4& mat, const vec3& r)
{
	rotateBy(mat, r, vec3());
}

inline void scaleBy(mat4& mat, const vec3& s)
{
	mat = Scale(s) * mat;
}

inline void reset(mat4& mat) noexcept { // option for a hard reset
	mat = mat4();
}
