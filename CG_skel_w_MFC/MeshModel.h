#pragma once
#include <string>
#include <vector>
#include "Entity.h"
#include "mat.h"
#include "Material.h"
#include "PrimTypes.h"
#include "vec.h"

class MeshModel : public Entity
{
protected:
	// data as imported from obj - in model space
	std::vector<Vertex> vertices;
	vec3 bounding_box_min;
	vec3 bounding_box_max;
	std::string name;

	MeshModel() noexcept;

	void fitBoundingBox();
	void processRawVerts(const std::vector<vec3>& verts, const std::vector<vec3>& normals, const std::vector<Face>& faces);
public:

	Material* material = Material::DefaultMaterial();
	bool draw_normals_per_vert = false;
	bool draw_normals_per_face = false;
	bool draw_bounding_box = false;
	bool draw_wireframe = false;

	MeshModel(std::string fileName, std::string modelName);
	~MeshModel();

	void loadFile(std::string fileName);
	
	vec3 getBoundingBoxMin() { return bounding_box_min; }
	vec3 getBoundingBoxMax() { return bounding_box_max; }
	std::vector<Vertex>* getVertices() { return &vertices; }
	std::string getName() { return name; }

	void Recenter();
};
