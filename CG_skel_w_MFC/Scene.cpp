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
