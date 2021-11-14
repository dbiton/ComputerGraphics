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
	vec3 face_mid;
	vec3 face_normal;

	Face();
	Face(int v0, int v1, int v2);
};

struct BoundingBox {
	vec3 box_min;
	vec3 box_max;
};

struct RenderVertices {
	std::vector<vec3> triangles;		// {vert0, vert1, vert2, vert3, ...} - each 3 pairs of verts are a triangle
	std::vector<vec3> triangles_colors;	// color for each vert
	std::vector<vec3> normals;			// {src0, dst0, src1, dst1, ...}
	std::vector<vec3> normals_colors;	// color for src and dst of normal

	void pushTriangle(vec3 v0, vec3 v1, vec3 v2, vec3 c0, vec3 c1, vec3 c2);
	void pushNormal(vec3 src, vec3 dir, vec3 c_src, vec3 c_dst);
	void clear() {
		triangles.clear();
		triangles_colors.clear();
		normals.clear();
		normals_colors.clear();
	}
};

class MeshModel : public Model
{
protected:
	// data as imported from obj - in model space
	std::vector<vec3> verts;
	std::vector<Face> faces;
	// bounding box in models space
	BoundingBox bounding_box;

	bool is_draw_normals_per_vert;
	bool is_draw_normals_per_face;
	bool is_draw_bounding_box;

	vec3 color_mesh;
	vec3 color_vert_normal;
	vec3 color_face_normal; 
	vec3 color_bounding_box;
	
	// data in world space
	RenderVertices renderVertices;
protected:
	MeshModel();

	void fitBoundingBox();
	void calculateFaceNormals();
	void calculateTriangles();
public:

	MeshModel(string fileName);
	~MeshModel(void);

	void loadFile(string fileName);
	
	void setDrawNormalsPerVert(bool b);
	void setDrawNormalsPerFace(bool b);
	void setDrawBoundingBox(bool b);

	virtual void draw(Renderer* renderer) override;
private:
	inline void getFaceVerts(const Face& face, vec3 &v0, vec3 &v1, vec3 &v2);
	inline void getFaceVertNormals(const Face& face, vec3& v0, vec3& v1, vec3& v2);
};
