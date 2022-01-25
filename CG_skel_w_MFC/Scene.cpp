#include "stdafx.h"

#include <string>
#include <cmath>
#include <cassert>
#include "MeshModel.h"
#include "PrimMeshModel.h"
#include "GL\freeglut.h"
#include "InitShader.h"
#include "Scene.h"

using namespace std;

mat4 Scene::Projection() {
    Camera* camera = getActiveCamera();
    const float midHeight = (camera->lastTop + camera->lastBottom) / 2,
        /*    */midWidth = (camera->lastLeft + camera->lastRight) / 2,
        /*    */dh = (camera->lastTop - camera->lastBottom) / 2 * (height / firstHeight),
        /*    */dw = (camera->lastLeft - camera->lastRight) / 2 * (width / firstWidth);
    if (camera->lastType == PROJECTION_ORTHO) {
        return camera->Ortho(midWidth + dw, midWidth - dw, midHeight + dh, midHeight - dh, camera->lastNear, camera->lastFar, false);
    }
    else {
        return camera->Frustum(midWidth + dw, midWidth - dw, midHeight + dh, midHeight - dh, camera->lastNear, camera->lastFar, false);
    }
}

void Scene::AddModel(MeshModel* model) {
    models.push_back(model);
    activeModel = models.size() - 1;
    if (activeCamera == -1) { // happens if this is the first model we're loading. so we set up a default scene for it
        cameras.push_back(Camera::DefaultCamera(model->getBoundingBoxMin(), model->getBoundingBoxMax()));
        activeCamera = 0;
        cameras[0]->LookAt(getPosition(cameras[0]->getTransform()), getPosition(model->getTransform()), vec4(0, 1, 0, 0));
        shading = SHADE_PHONG;
    }
    if (activeLight == -1) {
        lights.push_back(new AmbientLight(Color(1), 0.5));
        lights.push_back(new PointLight(Color(1), 2, 1.1 * model->getBoundingBoxMax()));
        activeLight = 1;
    }
    moveBy(model->world, getActiveCamera()->getLookingAt()); // spawn the model where the camera's looking
    draw();
}

Scene::Scene(int firstWidth, int firstHeight) : firstWidth(firstWidth), firstHeight(firstHeight), width(firstWidth), height(firstHeight) { }

void Scene::UpdateDimensions(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
}

void Scene::loadOBJModel(string fileName, string modelName) {
    AddModel(new MeshModel(fileName, modelName));
}

void Scene::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    const float time = ((float)clock()) / CLOCKS_PER_SEC;
    shaderSetFloat("time", time);
    shaderSetVec3("viewPos", getPosition(getActiveCamera()->getTransform()));
    shaderSetMat4("projection", Projection());
    shaderSetMat4("view", getActiveCamera()->getTransform());

    for (int i = 0; i < lights.size() && i < 64; i++) {
        shaderSetLight(lights[i], i);
    }

    shaderSetInt("numLights", lights.size());
    shaderSetInt("shading", shading);
    shaderSetInt("backshadows", drawBackshadow);

    shaderSetFloat("toonThickness", toonBorderThickness);
    shaderSetInt("toonEffect", toonShades);
    shaderSetInt("colorEffect", colorAnimType);
    shaderSetInt("animationEffect", vertexAnimType);

    for (const auto& model : models) {
        shaderSetMaterial(model->material);
        shaderSetMat4("model", model->getTransform());
        shaderSetMat4("modelInverse", transpose(InverseTransform(model->getTransform())));
        shaderSetInt("hasTexture", model->hasTexture());
        if (!IsWireframeMode() && isToon) { // first tell the model to draw its silhouette
            shaderSetVec3("toonBorderColor", toonBorderColor); // piggyback time!
            shaderSetInt("toonSilhouette", 1);
            glCullFace(GL_FRONT);
            model->DrawSilhouette();
            glCullFace(GL_BACK);
        }
        shaderSetInt("toonSilhouette", 0);
        model->Draw();
    }
    
    glFlush();
    glutSwapBuffers();
}

void Scene::drawDemo() {
}

int Scene::AddCamera(const Camera& camera) {
    activeCamera = cameras.size();
    cameras.push_back(new Camera(camera));
    return activeCamera;
}

void Scene::AddLight(Light* light) {
    activeLight = lights.size();
    lights.push_back(light);
}

void Scene::UpdateActiveLight(Light* params) {
    delete lights[activeLight];
    lights[activeLight] = params;
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

void Camera::LookAt(const vec4& eye, const vec4& at, const vec4& up) {
    const vec4 n = normalize(eye - at);
    vec4 u = cross(up, n);
    if (length(u) < FLT_EPSILON) u = cross(up, n + vec4(1, 0, 0, 0)); // fix for when too close to colinear
    u = normalize(u);
    const vec4 v = normalize(cross(n, u)),
        /*   */t = vec4(0.0, 0.0, 0.0, 1.0);
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

mat4 Camera::Ortho(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar, bool remember) {
    if (remember) UpdateLastParameters(left, right, bottom, top, zNear, zFar, PROJECTION_ORTHO);
    return Scale(2.0 / (right - left), 2.0 / (top - bottom), 2.0 / (zNear - zFar))
           * Translate(-0.5 * (right + left), -0.5 * (top + bottom), 0.5 * (zNear + zFar));
}

mat4 Camera::Frustum(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar, bool remember) {
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

    return transpose(N * S * H);
}

mat4 Camera::Perspective(const float fovy, const float aspect, const float zNear, const float zFar, bool remember) {
    const float fovy_rad = fovy * M_PI / 180,
        /*    */top = zNear * std::tan(fovy_rad / 2),
        /*    */bottom = -top,
        /*    */right = top * aspect,
        /*    */left = -right;
    return Camera::Frustum(left, right, bottom, top, zNear, zFar, remember);
}

void Camera::getPerspectiveParameters(float& fovy, float& aspect) {
    // distance to the the line defined as (left, y, near)
    float distance_to_left = std::sqrt(lastLeft * lastLeft + lastNear * lastNear);
    fovy = 2 * std::atan2(lastTop, distance_to_left) / M_PI * 180;
    aspect = (lastRight - lastLeft) / (lastTop - lastBottom);
}

void Light::setColor(Color _color) {
    color = _color;
}

void Light::setBrightness(float _brightness) {
    brightness = _brightness;
}

Color Light::getColor() const {
    return color;
}

float Light::getBrightness() const {
    return brightness;
}

int Light::getType() const {
    return type;
}

std::string Light::getNameOfType() const {
    switch (type) {
    case LIGHT_AMBIENT: return "Ambient";
    case LIGHT_POINT: return "Point";
    case LIGHT_PARALLEL: return "Parallel";
    default: return "UNKNOWN LIGHT TYPE";
    }
}

void PointLight::setPosition(vec3 _position) {
    posOrDir = _position;
}

void ParallelLight::setDirection(vec3 _direction) {
    posOrDir = normalize(_direction);
}

void Scene::setFog(bool enable, const Color color, float min, float max) // TODO
{
    isFog = enable;
    if (isFog) {
        colorFog = color;
        minDistanceFog = min;
        maxDistanceFog = max;
    }
    else {

    }
}

void Scene::setSupersampling(bool enable, int factor) // TODO
{
    isSupersample = enable;
    if (isSupersample) {
        factorSupersample = factor;
    }
    else {

    }
}

void Scene::setBloom(bool enable, float thresh, int spread) // TODO
{
    isBloom = enable;
    if (isBloom) {
        threshBloom = thresh;
        spreadBloom = spread;
    }
    else {

    }
}

void Scene::setColorAnim(bool enable, int type)
{
    isColorAnim = enable;
    colorAnimType = enable ? type : COLOR_ANIM_NONE;
}

void Scene::setVertexAnim(bool enable, int type)
{
    isVertexAnim = enable;
    vertexAnimType = enable ? type : VERTEX_ANIM_NONE;
}

void Scene::setToon(bool enable, int shades, float borderThickness, Color borderColor)
{
    isToon = enable;
    if (enable) {
        toonShades = shades;
        toonBorderThickness = borderThickness;
        toonBorderColor = borderColor;
    }
    else {
        toonShades = 0;
        toonBorderThickness = 0;
        toonBorderColor = 0;
    }
}

float Scene::getThreshBloom() const
{
    return threshBloom;
}

int Scene::getSpreadBloom() const
{
    return spreadBloom;
}

float Scene::getSupersamplingFactor() const
{
    return factorSupersample;
}

Color Scene::getFogColor() const
{
    return colorFog;
}

float Scene::getFogMinDistance() const
{
    return minDistanceFog;
}

float Scene::getFogMaxDistance() const
{
    return maxDistanceFog;
}

int Scene::getColorAnimType() const {
    return colorAnimType;
}

int Scene::getVertexAnimType() const {
    return vertexAnimType;
}

int Scene::getToonShades() const {
    return toonShades;
}

Color Scene::getToonBorderColor() const {
    return toonBorderColor;
}

float Scene::getToonBorderThickness() const {
    return toonBorderThickness;
}
