#pragma once

#include <string>
#include <vector>
#include "GL/glew.h"
#include "MeshModel.h"
#include "Renderer.h"

enum {
	LIGHT_AMBIENT,
	LIGHT_POINT,
	LIGHT_PARALLEL
};

class Light {
	Color color;
	float brightness;

protected:
	int type;
	Light(Color color, float brightness, int type) : color(color), brightness(brightness), type(type) { }

public:

	void setColor(Color _color);
	void setBrightness(float _brightness);

	Color getColor() const;
	float getBrightness() const;
	int getType() const;
	std::string getNameOfType() const;
	virtual vec3 dirToSource(const vec3& p) const = 0;
};

class AmbientLight : public Light {
public:
	AmbientLight(Color color, float brightness)
		: Light(color, brightness, LIGHT_AMBIENT) { }
	virtual vec3 dirToSource(const vec3& p) const;
};

class PointLight : public Light {
	vec3 position;
public:
	PointLight(Color color, float brightness, vec3 position)
		: Light(color, brightness, LIGHT_POINT), position(position) { }
	void setPosition(vec3 _position);
	vec3 getPosition() const { return position; }

	virtual vec3 dirToSource(const vec3& p) const;
};

class ParallelLight : public Light {
	vec3 direction;
public:
	ParallelLight(Color color, float brightness, vec3 direction)
		: Light(color, brightness, LIGHT_PARALLEL), direction(direction) { }
	void setDirection(vec3 _direction);
	vec3 getDirection() const { return direction; }

	virtual vec3 dirToSource(const vec3& p) const;
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

	static Camera* DefaultCamera(vec3 boxMin, vec3 boxMax) {
		Camera* res = new Camera();
		res->Ortho(-1, 1, -1, 1, 1, 3);
		mat4 transform = res->getTransform();
		setPosition(transform, boxMin + 2 * (boxMax - boxMin));
		res->self = transform;
		return res;
	}
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

	bool dimInactives = true;
	bool drawAxes = false;
	bool drawCameras = false;
	bool drawLights = false;

	Scene(Renderer *_renderer) : renderer(_renderer) { }

	void loadOBJModel(std::string fileName, std::string modelName);
	void draw();
	void drawDemo();
	
	MeshModel* getActiveModel() noexcept { return models[activeModel]; }
	Light* getActiveLight() noexcept { return lights[activeLight]; }
	Camera* getActiveCamera() noexcept { return cameras[activeCamera]; }

	std::vector<MeshModel*>* getModels() noexcept { return &models; }
	std::vector<Light*>* getLights() noexcept { return &lights; }
	std::vector<Camera*>* getCameras() noexcept { return &cameras; }

	int AddCamera(const Camera& camera);
	void AddLight(Light* light);
	void UpdateActiveLight(Light* params);

	void AddCuboid(vec3 p, vec3 dim);
	void AddPyramid(vec3 p, GLfloat height, GLfloat base_radius, int base_sides);
	void AddPrism(vec3 p, GLfloat height, GLfloat base_radius, int base_sides);
	void AddSphere(vec3 p, GLfloat radius, int subdivisions);
	void focus();
};
