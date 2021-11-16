#pragma once
#include "MeshModel.h"

class PrimMeshModel : public MeshModel
{
protected:
	PrimMeshModel();
public:
	static PrimMeshModel Cube(vec3 p, GLfloat len);
	static PrimMeshModel Cuboid(vec3 p, vec3 dim);
	static PrimMeshModel Pyramid(vec3 p, GLfloat height, GLfloat base_radius, int base_sides);
	static PrimMeshModel Prism(vec3 p, GLfloat height, GLfloat base_radius, int base_sides);
	static PrimMeshModel Sphere(vec3 p, GLfloat radius, int subdivisions);
};