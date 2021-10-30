#pragma once
#include "scene.h"
#include "vec.h"
#include "mat.h"
#include <string>

using namespace std;

struct Face {
	int v[3];
	int vn[3];
	int vt[3];
	
	// additional data - prevents the need to recalculate these values
	// once per frame when we we draw face normals
	vec3 face_mid;
	vec3 face_normal;

	Face();
	Face(int v0, int v1, int v2);
};

struct BoundingBox {
	vec3 box_min;
	vec3 box_max;
};

class MeshModel : public Model
{
protected:
	std::vector<vec3> verts;
	std::vector<Face> faces;

	BoundingBox bounding_box;

	bool is_draw_normals_per_vert;
	bool is_draw_normals_per_face;
	bool is_draw_bounding_box;

	mat4 world_transform;
protected:
	MeshModel();

	void fitBoundingBox();
	void calculateFaceNormals();

public:

	MeshModel(string fileName);
	~MeshModel(void);

	void loadFile(string fileName);
	
	void setDrawNormalsPerVert(bool b);
	void setDrawNormalsPerFace(bool b);
	void setDrawBoundingBox(bool b);

	virtual void draw() override;
};
