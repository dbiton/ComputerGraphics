#pragma once
#include "PrimTypes.h"
#include "Entity.h"
#include "scene.h"
#include "vec.h"
#include "mat.h"
#include <string>

using namespace std;

class Renderer;

class MeshModel : public Entity
{
protected:
	// data as imported from obj - in model space
	std::vector<Vertex> vertices;
	vec3 bounding_box_min;
	vec3 bounding_box_max;

	bool is_draw_normals_per_vert;
	bool is_draw_normals_per_face;
	bool is_draw_bounding_box;

	vec3 color_mesh;
	vec3 color_vert_normal;
	vec3 color_face_normal; 
	vec3 color_bounding_box;
protected:
	MeshModel() noexcept;

	void fitBoundingBox();
	void processRawVerts(const std::vector<vec3>& verts, const std::vector<Face>& faces);
public:

	MeshModel(string fileName);
	~MeshModel(void);

	void loadFile(string fileName);
	
	void SetDrawNormalsPerVert(bool b);
	void SetDrawNormalsPerFace(bool b);
	void SetDrawBoundingBox(bool b);

	void ToggleDrawNormalsPerVert();
	void ToggleDrawNormalsPerFace();
	void ToggleDrawBoundingBox();

	virtual void draw(Renderer* renderer);
};
