#pragma once

#include "gl/glew.h"
#include <vector>
#include <string>
#include "MeshModel.h"
#include "Renderer.h"


class MeshModel;
class Renderer;

class Light : public Entity{
	Color color;
	float brightness;
};

class PointLight : public Light {
	vec3 position;
};

class ParallelLight : public Light {
	vec3 direction;
};

enum {
	PROJECTION_ORTHO,
	PROJECTION_FRUSTUM
};

class Camera : public Entity{
	vec4 lookingAt;
	vec4 upDirection;

	void UpdateLastParameters(const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar, const int type);

public:
	Camera();

	mat4 projection;
	float lastBottom, lastTop, lastLeft, lastRight, lastNear, lastFar;
	int lastType; // all of this is needed to update the projection matrix upon reshape

	void LookAt(const vec4& eye, const vec4& at, const vec4& up );
	mat4 Ortho( const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar, bool remember = true);
	mat4 Frustum( const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar, bool remember = true);
	mat4 Perspective( const float fovy, const float aspect,
		const float zNear, const float zFar, bool remember = true);
	const vec4& getLookingAt() const { return lookingAt; }
	const vec4& getUpDirection() const { return upDirection; }
	void getPerspectiveParameters(float& fovy, float& aspect);
};

class Scene {
	std::vector<MeshModel*> models;
	std::vector<Light*> lights;
	std::vector<Camera*> cameras;
	Renderer* renderer;

	void AddModel(MeshModel* model);
	mat4 Projection();

public:
	bool dimInactiveModels = true;
	bool drawAxes = false;
	bool drawCameras = false;

	Scene(Renderer *_renderer) : renderer(_renderer), activeModel(-1), activeLight(-1), activeCamera(-1) { };

	void loadOBJModel(std::string fileName);
	void draw();
	void drawDemo();
	
	int activeModel;
	int activeLight;
	int activeCamera;

	MeshModel* getActiveModel() { return models[activeModel]; }
	Light* getActiveLight() { return lights[activeLight]; }
	Camera* getActiveCamera() { return cameras[activeCamera]; }

	int AddCamera(const Camera& camera);

	void AddCuboid(vec3 p, vec3 dim);
	void AddPyramid(vec3 p, GLfloat height, GLfloat base_radius, int base_sides);
	void AddPrism(vec3 p, GLfloat height, GLfloat base_radius, int base_sides);
	void AddSphere(vec3 p, GLfloat radius, int subdivisions);
	void focus();

	// might just make the vectors public lol
};
