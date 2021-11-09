#include "stdafx.h"
#include "Scene.h"
#include "MeshModel.h"
#include <string>

using namespace std;
void Scene::loadOBJModel(string fileName)
{
	MeshModel *model = new MeshModel(fileName);
	models.push_back(model);
}

void Scene::draw()
{
	// 1. Send the renderer the current camera transform and the projection
	// 2. Tell all models to draw themselves
	renderer->SetCameraTransform(cameras[activeCamera]->getTransform());
	renderer->SetProjection(cameras[activeCamera]->getProjection());
	for (const auto& model : models) {
		model->draw(renderer);
	}
	renderer->SwapBuffers();
}

void Scene::drawDemo()
{
	renderer->SetDemoBuffer();
	renderer->SwapBuffers();
}

void Camera::LookAt(const vec4& eye, const vec4& at, const vec4& up)
{
}

void Camera::Ortho(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar)
{
}

void Camera::Frustum(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar)
{
}

mat4 Camera::Perspective(const float fovy, const float aspect, const float zNear, const float zFar)
{
	return mat4();
}

const mat4& Camera::getProjection() const
{
	return mat4();
}
