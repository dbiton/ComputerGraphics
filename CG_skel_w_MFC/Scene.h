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
		cameras[0]->LookAt(vec4(5, 1, 2, 1), vec4(0, 0, 0, 0), vec4(0, 0, 1, 1));
		//cameras[0]->Ortho(-5, 5, -5, 5, -5, 5);
		cameras[0]->Perspective(90.f, 1.f, 1, 2);
		activeCamera = 0;
	};

	void loadOBJModel(std::string fileName);
	void draw();
	void drawDemo();
	
	Camera& getActiveCamera();

	int activeModel;
	int activeLight;
	int activeCamera;
};