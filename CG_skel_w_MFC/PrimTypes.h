#pragma once

#include "vec.h"
#include <GL/GL.h>

typedef vec3 Color;

struct Vertex {
	vec3 position;
	vec3 normal;
	vec2 tex;
};

struct Face {
	int v[3];
	int vn[3];
	int vt[3];

	Face();
	Face(int v0, int v1, int v2);
};
