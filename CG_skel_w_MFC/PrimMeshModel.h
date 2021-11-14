#pragma once
#include "MeshModel.h"

class PrimMeshModel : public MeshModel
{
protected:
	PrimMeshModel();
public:
	static PrimMeshModel Cube(vec3 p, GLfloat len);
	static PrimMeshModel Box(vec3 p, vec3 dim);
};