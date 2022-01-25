#pragma once

#include "vec.h"
#include <GL/GL.h>

typedef vec3 Color;

struct Vertex {
	vec3 position;
	vec3 normal;
	vec3 avg_normal;
	vec3 face_normal;
	vec3 face_middle;
	vec2 tex;
};

struct NormalSum {
	vec3 sum;
	int count;
};

struct Face {
	int v[3];
	int vn[3];
	int vt[3];

	Face();
	Face(int v0, int v1, int v2);
};
