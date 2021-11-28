#include "stdafx.h"
#include "Scene.h"
#include "MeshModel.h"
#include "PrimMeshModel.h"
#include <string>
#include <cmath>
#include <cassert>

using namespace std;

void Scene::AddModel(MeshModel* model) {
    models.push_back(model);
    activeModel = models.size() - 1;
    if (activeCamera == -1) { // happens if this is the first model we're loading. so we set up a default frustum camera to look at it
        vec3 min = getActiveModel()->getBoundingBoxMin(),
             max = getActiveModel()->getBoundingBoxMax();
        cameras.push_back(new Camera());
        activeCamera = 0;
        getActiveCamera()->Perspective(90, 1, 1, 8);
        mat4 transform = getActiveCamera()->getTransform();
        setPosition(transform, vec3(0, 0, min.z - 2));
        getActiveCamera()->self = transform;
    }
    moveBy(model->world, getActiveCamera()->getLookingAt()); // spawn the model where the camera's looking
    draw();
}

void Scene::loadOBJModel(string fileName)
{
    AddModel(new MeshModel(fileName));
}

mat4 Scene::Projection() {
    Camera* camera = getActiveCamera();
    const float midHeight = (camera->lastTop + camera->lastBottom) / 2,
                midWidth = (camera->lastLeft + camera->lastRight) / 2,
                dh = (camera->lastTop - camera->lastBottom) / 2 * renderer->GetHeightMultiplier(),
                dw = (camera->lastLeft - camera->lastRight) / 2 * renderer->GetWidthMultiplier();
    if (camera->lastType == PROJECTION_ORTHO) {
        return camera->Ortho(midWidth + dw, midWidth - dw, midHeight + dh, midHeight - dh, camera->lastNear, camera->lastFar, false);
    }
    else {
        return camera->Frustum(midWidth + dw, midWidth - dw, midHeight + dh, midHeight - dh, camera->lastNear, camera->lastFar, false);
    }
}

void Scene::draw()
{
    renderer->SetCameraTransform(getActiveCamera()->getTransform());
    renderer->SetProjection(Projection());

    if (drawAxes) renderer->DrawAxes();
    if (drawCameras) {
        for (int i = 0; i < cameras.size(); i++) {
            if (i != activeCamera) {
                renderer->DrawCamera(cameras[i]);
            }
        }
    }
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

int Scene::AddCamera(const Camera& camera) {
    activeCamera = cameras.size();
    cameras.push_back(new Camera(camera));
    return activeCamera;
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

void Camera::UpdateLastParameters(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar, const int type) {
    lastBottom = bottom;
    lastTop = top;
    lastLeft = left;
    lastRight = right;
    lastNear = zNear;
    lastFar = zFar;
    lastType = type;
}

Camera::Camera()
{
}

mat4 Camera::Ortho(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar, bool remember)
{
    if (remember) UpdateLastParameters(left, right, bottom, top, zNear, zFar, PROJECTION_ORTHO);
    return Scale(2.0 / (right - left), 2.0 / (top - bottom), 2.0 / (zNear - zFar))
           * Translate(-0.5 * (right + left), -0.5 * (top + bottom), 0.5 * (zNear + zFar));
}

mat4 Camera::Frustum(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar, bool remember)
{
    if (remember) UpdateLastParameters(left, right, bottom, top, zNear, zFar, PROJECTION_FRUSTUM);
    mat4 H = mat4();
    H[2][0] = (left + right) / (2.0 * zNear);
    H[2][1] = (top + bottom) / (2.0 * zNear);

    const mat4 S = Scale(2.0 * zNear / (right - left), 2.0 * zNear / (top - bottom), 1);

    mat4 N = mat4();
    N[2][2] = (zNear + zFar) / (zNear - zFar);
    N[3][2] = 2.0 * zNear * zFar / (zNear - zFar);
    N[2][3] = -1;
    N[3][3] = 0;

    return N * S * H;
}

mat4 Camera::Perspective(const float fovy, const float aspect, const float zNear, const float zFar, bool remember)
{
    float fovy_rad = fovy * 180 / M_PI;
    float left, right, bottom, top;
    top = zNear * std::tan(fovy_rad / 2);
    bottom = -top;
    right = top / aspect;
    left = -right;
    return Camera::Frustum(left, right, bottom, top, zNear, zFar, remember);
}

void Camera::getPerspectiveParameters(float& fovy, float& aspect) {
    // distance to the the line defined as (left, y, near)
    float distance_to_left = std::sqrt(lastLeft * lastLeft + lastNear * lastNear);
    fovy = 2 * std::atan2(lastTop, distance_to_left) / M_PI * 180;
    aspect = (lastTop - lastBottom) / (lastRight - lastLeft);
}

Light::Light()
{
}

void Light::setColor(Color _color)
{
    color = _color;
}

void Light::setBrightness(float _brightness)
{
    brightness = _brightness;
}

Color Light::getColor() const
{
    return color;
}

float Light::getBrightness() const
{
    return brightness;
}

vec3 Light::dirToSource(vec3 p) const
{
    // the Light class is intended to only be used for ambient light, it has no direction.
    return vec3();
}

void PointLight::setPosition(vec3 _position)
{
    position = _position;
}

vec3 PointLight::dirToSource(vec3 p) const
{
    return normalize(position - p);
}

void ParallelLight::setDirection(vec3 _direction)
{
    // check that _direction is normalized (or close enough to it)
    assert(std::abs(length(_direction) - 1) < FLT_EPSILON);
    direction = _direction;
}

vec3 ParallelLight::dirToSource(vec3 p) const
{
    return direction;
}
