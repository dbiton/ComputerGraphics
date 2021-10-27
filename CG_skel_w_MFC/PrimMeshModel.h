#pragma once
#include "MeshModel.h"

// implementation to be determined once we know if we are
// allowed to use quads
class PrimMeshModel : public MeshModel
{
protected:
	PrimMeshModel();
public:
	PrimMeshModel Cube(vec3 p, GLfloat len);
	PrimMeshModel Box(vec3 p, vec3 dim);
};

