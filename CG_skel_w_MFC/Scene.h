#pragma once

#include <string>
#include <vector>
#include "GL/glew.h"
#include "MeshModel.h"

enum {
    LIGHT_AMBIENT,
    LIGHT_POINT,
    LIGHT_PARALLEL
};

struct Light {
    Color ambient = Color(1, 0, 0);
    Color diffuse = Color(0, 1, 0);
    Color specular = Color(0, 0, 1);
    vec3 position = vec3(1, 1, 1);
    float brightness = 1;
    bool isDirectional = false; // if this is true, position is actually the direction the light is coming from

    static Light PointLight(Color ambient, Color diffuse, Color specular, vec3 position, float brightness);
    static Light DirectionalLight(Color ambient, Color diffuse, Color specular, vec3 position, float brightness);

    std::string getNameOfType() const;
};

enum {
    PROJECTION_ORTHO,
    PROJECTION_FRUSTUM
};

class Camera : public Entity {
    vec4 lookingAt;
    vec4 upDirection;

    void UpdateLastParameters(const float left, const float right,
        const float bottom, const float top,
        const float zNear, const float zFar, const int type);

public:
    Camera() { }

    mat4 projection;
    float lastBottom, lastTop, lastLeft, lastRight, lastNear, lastFar;
    int lastType; // all of this is needed to update the projection matrix upon reshape

    void LookAt(const vec4& eye, const vec4& at, const vec4& up);
    mat4 Ortho(const float left, const float right,
        const float bottom, const float top,
        const float zNear, const float zFar, bool remember = true);
    mat4 Frustum(const float left, const float right,
        const float bottom, const float top,
        const float zNear, const float zFar, bool remember = true);
    mat4 Perspective(const float fovy, const float aspect,
        const float zNear, const float zFar, bool remember = true);
    const vec4& getLookingAt() const noexcept { return lookingAt; }
    const vec4& getUpDirection() const noexcept { return upDirection; }
    void getPerspectiveParameters(float& fovy, float& aspect);

    static Camera* DefaultCamera(vec3 boxMin, vec3 boxMax) {
        Camera* res = new Camera();
        res->projection = res->Ortho(-1, 1, -1, 1, 1, 3);
        mat4 transform = res->getTransform();
        setPosition(transform, boxMin + 2 * (boxMax - boxMin));
        res->self = transform;
        return res;
    }
};

enum {
    SHADE_NONE,
    SHADE_FLAT,
    SHADE_GOURAUD,
    SHADE_PHONG
};

class Scene {
    std::vector<MeshModel*> models;
    std::vector<Light*> lights;
    std::vector<Camera*> cameras;

    void AddModel(MeshModel* model);

    bool isBloom, isFog, isSupersample, isColorAnim, isVertexAnim, isToon;
    int spreadBloom = 10;
    float threshBloom = 0.5;
    Color colorFog = Color(1);
    float minDistanceFog = 0, maxDistanceFog = 100;
    int factorSupersample = 2;
    int colorAnimType = 1;
    int vertexAnimType = 1;
    int toonShades = 5;

public:
    int shading = 0;

    int activeModel = -1;
    int activeLight = -1;
    int activeCamera = -1;

    bool dimInactives = true;
    bool drawAxes = false;
    bool drawCameras = false;
    bool drawLights = false;

    Scene();

    void loadOBJModel(std::string fileName, std::string modelName);
    void draw();
    void drawDemo();

    MeshModel* getActiveModel() noexcept { return models[activeModel]; }
    Light* getActiveLight() noexcept { return lights[activeLight]; }
    Camera* getActiveCamera() noexcept { return cameras[activeCamera]; }

    std::vector<MeshModel*>* getModels() noexcept { return &models; }
    std::vector<Light*>* getLights() noexcept { return &lights; }
    std::vector<Camera*>* getCameras() noexcept { return &cameras; }

    int AddCamera(const Camera& camera);
    void AddLight(Light* light);
    void UpdateActiveLight(Light* params);

    void AddCuboid(vec3 p, vec3 dim);
    void AddPyramid(vec3 p, GLfloat height, GLfloat base_radius, int base_sides);
    void AddPrism(vec3 p, GLfloat height, GLfloat base_radius, int base_sides);
    void AddSphere(vec3 p, GLfloat radius, int subdivisions);
    void focus();

    void setFog(bool enable, Color color = Color(0.f), float min = 0.f, float max = 0.f);
    void setSupersampling(bool enable, int factor = 0);
    void setBloom(bool enable, float thresh = 0.f, int spread = 0);
    void setColorAnim(bool enable, int type = 0);
    void setVertexAnim(bool enable, int type = 0);
    void setToon(bool enable, int shades = 0);

    float getThreshBloom() const;
    int getSpreadBloom() const;

    float getSupersamplingFactor() const;

    Color getFogColor() const;
    float getFogMinDistance() const;
    float getFogMaxDistance() const;

    int getColorAnimType() const;
    int getVertexAnimType() const;
    int getToonShades() const;
};
