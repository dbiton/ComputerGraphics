#pragma once

#include "vec.h"
#include "PrimTypes.h"

struct Material {
	Color color;

	float ambient_reflect;
	float roughness;
	float shininess;

	static Material DefaultMaterial()
	{
		return Material{ Color(0.5), 0.5, 0.5, 0.5 };
	}
};