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
        vec3 min = getActiveModel()->getBoundingBoxMin(),
             max = getActiveModel()->getBoundingBoxMax();
        cameras.push_back(new Camera());
        activeCamera = 0;
        getActiveCamera()->LookAt(getPosition(getActiveModel()->world) + max * vec3(3, 1, 0.5), getPosition(getActiveModel()->world), vec4(0, 0, 1, 1));
        getActiveCamera()->Frustum(min.x * 2, max.x * 2, min.x * 2, max.x * 2, min.z, max.z);
    }
    moveBy(model->world, getActiveCamera()->getLookingAt()); // spawn the model where the camera's looking
    draw();
}

void Scene::loadOBJModel(string fileName)
{
    AddModel(new MeshModel(fileName));
}

void Scene::draw()
{
    renderer->SetCameraTransform(getActiveCamera()->getTransform());
    renderer->SetProjection(getActiveCamera()->getProjection());

    if (drawAxes) renderer->DrawAxes();
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

void Scene::AddCuboid(vec3 p, vec3 dim) {
    AddModel(new MeshModel(PrimMeshModel::Cuboid(p, dim)));
}

void Scene::AddPyramid(vec3 p, GLfloat height, GLfloat base_radius, int base_sides) {
    AddModel(new MeshModel(PrimMeshModel::Pyramid(p, height, base_radius, base_sides)));
}

void Scene::AddPrism(vec3 p, GLfloat height, GLfloat base_radius, int base_sides) {
    AddModel(new MeshModel(PrimMeshModel::Prism(p, height, base_radius, base_sides)));
}

void Scene::AddSphere(vec3 p, GLfloat radius, int subdivisions) {
    AddModel(new MeshModel(PrimMeshModel::Sphere(p, radius, subdivisions)));
}

void Scene::focus() {
    getActiveCamera()->LookAt(getPosition(getActiveCamera()->getTransform()), getPosition(getActiveModel()->getTransform()), getActiveCamera()->getUpDirection());
}

// TODO check that all of these are the right order!

void Camera::LookAt(const vec4& eye, const vec4& at, const vec4& up)
{ // TODO for some reason this math is not idempotent...
    const vec4 n = normalize(eye - at),
               u = normalize(cross(up, n)),
               v = normalize(cross(n, u)),
               t = vec4(0.0, 0.0, 0.0, 1.0);
    lookingAt = at;
    upDirection = up;

    reset(self);
    world = mat4(u, v, n, t) * Translate(-eye);
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

    const mat4 S = Scale(2.0 * zNear / (right - left), 2.0 * zNear / (top - bottom), 1);

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
