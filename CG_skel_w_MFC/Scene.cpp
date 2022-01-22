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

void Scene::AddModel(MeshModel* model) {
    models.push_back(model);
    activeModel = models.size() - 1;
    if (activeCamera == -1) { // happens if this is the first model we're loading. so we set up a default scene for it
        cameras.push_back(Camera::DefaultCamera(model->getBoundingBoxMin(), model->getBoundingBoxMax()));
        activeCamera = 0;
        cameras[0]->LookAt(getPosition(cameras[0]->getTransform()), vec4(), vec4(0, 0, 1, 0));
        const vec3 box = model->getBoundingBoxMax() - model->getBoundingBoxMin();
        const GLfloat scale = max(max(box.x, box.y), box.z);
        scaleBy(cameras[0]->world, vec3(1.0/scale));
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

Scene::Scene() { }

void Scene::loadOBJModel(string fileName, string modelName) {
    AddModel(new MeshModel(fileName, modelName));
}

void Scene::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLuint viewPos_loc = glGetUniformLocation(GetProgram(), "viewPos");
    glUniform3fv(viewPos_loc, 1, getPosition(getActiveCamera()->getTransform()));
    GLuint view_loc = glGetUniformLocation(GetProgram(), "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, getActiveCamera()->getTransform());
    GLuint projection_loc = glGetUniformLocation(GetProgram(), "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, getActiveCamera()->projection);
    for (const auto& model : models) {
        GLuint model_loc = glGetUniformLocation(GetProgram(), "model");
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, model->getTransform());
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
               t = vec4(0.0, 0.0, 0.0, 1.0);
    lookingAt = at;
    upDirection = up;

    reset(self);
    world = InverseTransform(mat4(u, v, n, t) * Translate(-eye)); // will be inverted again later!
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

    return N * S * H;
}

mat4 Camera::Perspective(const float fovy, const float aspect, const float zNear, const float zFar, bool remember) {
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
    position = _position;
}

vec3 PointLight::dirToSource(const vec3& p) const {
    return normalize(position - p);
}

void ParallelLight::setDirection(vec3 _direction) {
    // check that _direction is normalized (or close enough to it)
    assert(std::abs(length(_direction) - 1) < FLT_EPSILON);
    direction = _direction;
}

vec3 ParallelLight::dirToSource(const vec3& p) const {
    return direction;
}

vec3 AmbientLight::dirToSource(const vec3& p) const {
    // ambient light is by definition directionless, so...
    return vec3();
}

void Scene::setFog(bool enable, const Color color, float min, float max)
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

void Scene::setSupersampling(bool enable, int factor)
{
    isSupersample = enable;
    if (isSupersample) {
        factorSupersample = factor;
    }
    else {

    }
}

void Scene::setBloom(bool enable, float thresh, int spread)
{
    isBloom = enable;
    if (isBloom) {
        threshBloom = thresh;
        spreadBloom = spread;
    }
    else {

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

// TODO CODE FOR FIGURING OUT HOW OPENGL MATRIX TRANSFORMATION WORKS, DELETE LATER
/*
mat4 ViewMatrix;
mat4 ProjectionMatrix;

mat4 getViewMatrix() { return ViewMatrix; }
mat4 getProjectionMatrix() { return ProjectionMatrix; }

// Initial position : on +Z
vec3 position = vec3( 0, 0, 5 );
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;



void computeMatricesFromInputs(){

    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = 3;//glfwGetTime();

    // Compute time difference between current and last frame
    double currentTime = 3;// glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // Get mouse position
    double xpos, ypos;
    //glfwGetCursorPos(window, &xpos, &ypos);

    // Reset mouse position for next frame
    //glfwSetCursorPos(window, 1024/2, 768/2);

    // Compute new orientation
    horizontalAngle += mouseSpeed * float(1024/2 - xpos );
    verticalAngle   += mouseSpeed * float( 768/2 - ypos );

    // Direction : Spherical coordinates to Cartesian coordinates conversion
    vec3 direction(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

    // Right vector
    vec3 right = vec3(sin(horizontalAngle - 3.14f/2.0f), 0, cos(horizontalAngle - 3.14f/2.0f));

    // Up vector
    vec3 up = cross( right, direction );

    // Move forward
    //if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    //	position += direction * deltaTime * speed;
    //}
    //// Move backward
    //if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
    //	position -= direction * deltaTime * speed;
    //}
    //// Strafe right
    //if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
    //	position += right * deltaTime * speed;
    //}
    //// Strafe left
    //if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
    //	position -= right * deltaTime * speed;
    //}

    float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

    //// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    //ProjectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
    //// Camera matrix
    //ViewMatrix       = lookAt(
    //							position,           // Camera is here
    //							position+direction, // and looks here : at the same position, plus "direction"
    //							up                  // Head is up (set to 0,-1,0 to look upside-down)
    //					   );

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}
*/
