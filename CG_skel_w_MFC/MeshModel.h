#pragma once
#include "scene.h"
#include "vec.h"
#include "mat.h"
#include <string>

using namespace std;

// although we appear to allow for a quad (a face with 4 verts), we later assume we only use tris.
struct Face {
	int v[4];
	int vn[4];
	int vt[4];

	// additional data - prevents the need to recalculate these values
	// once per frame when we we draw face normals
	vec3 face_mid;
	vec3 face_normal;
};

struct BoundingBox {
	vec3 box_min;
	vec3 box_max;
};

class MeshModel : public Model
{
protected :
	MeshModel();

	std::vector<vec3> verts;
	std::vector<Face> faces;

	BoundingBox bounding_box;

	bool is_draw_normals_per_vert;
	bool is_draw_normals_per_face;
	bool is_draw_bounding_box;

	mat4 _world_transform;
	mat3 _normal_transform;

public:

	MeshModel(string fileName);
	~MeshModel(void);

	void loadFile(string fileName);
	
	void setDrawNormalsPerVert(bool b);
	void setDrawNormalsPerFace(bool b);
	void setDrawBoundingBox(bool b);

	void draw();
private:
	void fitBoundingBox();
	void calculateFaceNormals();
};
