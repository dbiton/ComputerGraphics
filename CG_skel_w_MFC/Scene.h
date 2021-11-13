#pragma once

#include "gl/glew.h"
#include <vector>
#include <string>
#include "Renderer.h"
#include "Entity.h"
using namespace std;

class Model : public Entity {
protected:
	virtual ~Model() {}
public:
	virtual void draw(Renderer* renderer) = 0;
};


class Light : public Entity{

};

class Camera : public Entity{
	mat4 projection;

public:
	void LookAt(const vec4& eye, const vec4& at, const vec4& up );
	void Ortho( const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar );
	void Frustum( const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar );
	mat4 Perspective( const float fovy, const float aspect,
		const float zNear, const float zFar);
	const mat4& getProjection() const;
};

class Scene {

	vector<Model*> models;
	vector<Light*> lights;
	vector<Camera*> cameras;
	Renderer *renderer;

public:
	Scene() {};
	Scene(Renderer *_renderer) : renderer(_renderer) {
		cameras.push_back(new Camera());
		activeCamera = 0;
	};
	void loadOBJModel(string fileName);
	void draw();
	void drawDemo();
	
	int activeModel;
	int activeLight;
	int activeCamera;
};