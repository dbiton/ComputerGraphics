#pragma once
#include <string>
#include <vector>
#include "Entity.h"
#include "mat.h"
#include "Material.h"
#include "PrimTypes.h"
#include "vec.h"
#include "stb_image.h"

class MeshModel : public Entity
{
protected:
	vec3 bounding_box_min;
	vec3 bounding_box_max;
	std::string name;

	size_t vao_size, vao_vNormals_size, vao_sNormals_size, vao_boundingBox_size;
	GLuint vao, vao_vNormals, vao_sNormals, vao_boundingBox;
	GLuint texture = -1;
	GLfloat rescale;
	
	MeshModel() noexcept;

	void processRawVerts(const std::vector<vec3>& positions, const std::vector<vec3>& normals, const std::vector<vec2>& texs, const std::vector<Face>& faces);

	static int default_uv;
public:
	Material* material = Material::DefaultMaterial();
	bool draw_normals_per_vert = false;
	bool draw_normals_per_face = false;
	bool draw_bounding_box = false;

	MeshModel(std::string fileName, std::string modelName);
	~MeshModel();

	bool hasTexture();
	void loadTexture(std::string path);
	void clearTexture();
	
	void loadFile(std::string fileName);
	
	vec3 getBoundingBoxMin() { return bounding_box_min; }
	vec3 getBoundingBoxMax() { return bounding_box_max; }
	std::string getName() { return name; }

	static void setDefaultUV(int _default_uv);
	static int getDefaultUV();

	void Draw();
	void Recenter();
private:
	vec2 genSphereUV(vec3 p);
	vec2 genPlaneUV(vec3 p);
};

enum { // TODO move this to the place that has the actual texture mapping code?
	UV_SPHERE,
	UV_PLANE,
	UV_CYLINDER,
	UV_BOX
};
