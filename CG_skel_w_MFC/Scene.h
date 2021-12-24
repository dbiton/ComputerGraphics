#pragma once

#include "gl/glew.h"
#include <vector>
#include <string>
#include "MeshModel.h"
#include "Renderer.h"

class MeshModel;
class Renderer;

class Light {
	Color color;
	float brightness;
public:
	Light() { }

	void setColor(Color _color);
	void setBrightness(float _brightness);

	Color getColor() const;
	float getBrightness() const;
	virtual vec3 dirToSource(vec3 p) const = 0;
};

class AmbientLight : public Light {
public:
	virtual vec3 dirToSource(vec3 p) const;
};

class PointLight : public Light {
	vec3 position;
public:
	void setPosition(vec3 _position);

	virtual vec3 dirToSource(vec3 p) const;
};

class ParallelLight : public Light {
	vec3 direction;
public:
	void setDirection(vec3 _direction);

	virtual vec3 dirToSource(vec3 p) const;
};

enum {
	PROJECTION_ORTHO,
	PROJECTION_FRUSTUM
};

class Camera : public Entity {
	vec4 lookingAt;
	vec4 upDirection;

	void UpdateLastParameters(const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar, const int type);

public:
	Camera() { }

	mat4 projection;
	int shading;
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
	const vec4& getLookingAt() const noexcept { return lookingAt; }
	const vec4& getUpDirection() const noexcept { return upDirection; }
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
	int activeModel = -1;
	int activeLight = -1;
	int activeCamera = -1;

	bool dimInactiveModels = true;
	bool drawAxes = false;
	bool drawCameras = false;

	Scene(Renderer *_renderer) : renderer(_renderer) { }

	void loadOBJModel(std::string fileName);
	void draw();
	void drawDemo();
	
	MeshModel* getActiveModel() noexcept { return models[activeModel]; }
	Light* getActiveLight() noexcept { return lights[activeLight]; }
	Camera* getActiveCamera() noexcept { return cameras[activeCamera]; }

	int AddCamera(const Camera& camera);

	void AddCuboid(vec3 p, vec3 dim);
	void AddPyramid(vec3 p, GLfloat height, GLfloat base_radius, int base_sides);
	void AddPrism(vec3 p, GLfloat height, GLfloat base_radius, int base_sides);
	void AddSphere(vec3 p, GLfloat radius, int subdivisions);
	void focus();

	// might just make the vectors public lol
};
