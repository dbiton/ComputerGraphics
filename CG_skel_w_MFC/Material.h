#pragma once

#include "vec.h"

struct Material {
	vec3 emissive;
	vec3 diffuse;
	vec3 specular;

	static Material DefaultMaterial() {
		return Material{ vec3(0.5), vec3(0.5), vec3(0.5) };
	}
};