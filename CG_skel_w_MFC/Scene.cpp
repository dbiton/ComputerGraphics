#include "stdafx.h"
#include "Scene.h"
#include "MeshModel.h"
#include "PrimMeshModel.h"
#include <string>

using namespace std;

void Scene::AddModel(MeshModel* model) {
    models.push_back(model);
    activeModel = models.size() - 1;
    if (activeCamera == -1) { // happens if this is the first model we're loading. so we set up a default frustum camera to look at it
        vec3 min = models[activeModel]->getBoundingBoxMin(),
             max = models[activeModel]->getBoundingBoxMax();
        cameras.push_back(new Camera());
        activeCamera = 0;
        cameras[0]->LookAt(models[activeModel]->getPosition() + max * vec3(3, 1, 0.5), models[activeModel]->getPosition(), vec4(0, 0, 1, 1));
        cameras[0]->Frustum(min.x * 2, max.x * 2, min.x * 2, max.x * 2, min.z, max.z);
    }
    draw();
}

void Scene::loadOBJModel(string fileName)
{
    MeshModel* model = new MeshModel(fileName);
    model->setPosition(vec3(0, 0, 0));
    AddModel(model);
}

void Scene::draw()
{
    // 1. Send the renderer the current camera transform and the projection
    // 2. Tell all models to draw themselves
    renderer->SetCameraTransform(cameras[activeCamera]->getTransform());
    renderer->SetProjection(cameras[activeCamera]->getProjection());
    for (int i = 0; i < models.size(); i++)
    {
        models[i]->draw(renderer, !dimInactiveModels || i == activeModel);
    }
    renderer->SwapBuffers();
}

void Scene::drawDemo()
{
    renderer->SetDemoBuffer();
    renderer->SwapBuffers();
}

void Scene::AddBox(vec3 p, vec3 dim) { // TODO this aint no box
    AddModel(new MeshModel(PrimMeshModel::Box(p, dim)));
}

// TODO check that all of these are the right order!

void Camera::LookAt(const vec4& eye, const vec4& at, const vec4& up)
{
    const vec4 n = normalize(eye - at),
               u = normalize(cross(up, n)),
               v = normalize(cross(n, u)),
               t = vec4(0.0, 0.0, 0.0, 1.0);
    setTransform(mat4(u, v, n, t) * Translate(-eye));
}

void Camera::Ortho(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar)
{
    projection = Scale(2.0 / (right - left), 2.0 / (top - bottom), 2.0 / (zNear - zFar))
                 * Translate(-0.5 * (right + left), -0.5 * (top + bottom), 0.5 * (zNear + zFar));
}

void Camera::Frustum(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar)
{
    mat4 H = mat4();
    H[2][0] = (left + right) / (2.0 * zNear);
    H[2][1] = (top + bottom) / (2.0 * zNear);

    mat4 S = Scale(2.0 * zNear / (right - left), 2.0 * zNear / (top - bottom), 1);

    mat4 N = mat4();
    N[2][2] = (zNear + zFar) / (zNear - zFar);
    N[3][2] = 2.0 * zNear * zFar / (zNear - zFar);
    N[2][3] = -1;
    N[3][3] = 0;

    projection = N * S * H;
}

mat4 Camera::Perspective(const float fovy, const float aspect, const float zNear, const float zFar)
{
    return mat4(); // TODO wth is this
}

const mat4& Camera::getProjection() const
{
    return projection;
}
