#pragma once

#include "gl/glew.h"
#include <vector>
#include <string>
#include "MeshModel.h"
#include "Renderer.h"


class MeshModel;
class Renderer;

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
	std::vector<MeshModel*> models;
	std::vector<Light*> lights;
	std::vector<Camera*> cameras;
	Renderer* renderer;

public:
	Scene(Renderer *_renderer) : renderer(_renderer) 
	{
		cameras.push_back(new Camera());
		activeCamera = 0;
	};

	void loadOBJModel(std::string fileName);
	void draw();
	void drawDemo();
	
	int activeModel;
	int activeLight;
	int activeCamera;
};