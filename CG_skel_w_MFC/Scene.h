#pragma once

#include <string>
#include <vector>
#include "GL/glew.h"
#include "MeshModel.h"

enum {
    LIGHT_AMBIENT = 0,
    LIGHT_POINT = 1,
    LIGHT_PARALLEL = 2
};

enum {
    COLOR_ANIM_NONE = 0,
    COLOR_ANIM_RGB = 1,
    COLOR_ANIM_STRIPES = 2,
    COLOR_ANIM_WOW = 3
};

enum {
    VERTEX_ANIM_NONE = 0,
    VERTEX_ANIM_SIMPLE = 1,
    VERTEX_ANIM_WOW = 2
};

enum {
    SHADE_FLAT = 0,
    SHADE_GOURAUD = 1,
    SHADE_PHONG = 2
};

struct Light {
    Color color;
    float brightness;
    int type;

protected:
    Light(vec3 posOrDir, Color color, float brightness, int type) : posOrDir(posOrDir), color(color), brightness(brightness), type(type) { }

public:
    vec3 posOrDir;

    void setColor(Color _color);
    void setBrightness(float _brightness);
    Color getColor() const;
    float getBrightness() const;
    int getType() const;
    std::string getNameOfType() const;
};

class AmbientLight : public Light {
public:
    AmbientLight(Color color, float brightness)
        : Light(vec3(0), color, brightness, LIGHT_AMBIENT) { }
};

class PointLight : public Light {
public:
    PointLight(Color color, float brightness, vec3 position)
        : Light(position, color, brightness, LIGHT_POINT) { }
    void setPosition(vec3 _position);
    vec3 getPosition() const { return posOrDir; }
};

class ParallelLight : public Light {
public:
    ParallelLight(Color color, float brightness, vec3 direction)
        : Light(direction, color, brightness, LIGHT_PARALLEL) { }
    void setDirection(vec3 _direction);
    vec3 getDirection() const { return posOrDir; }
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
        res->projection = res->Perspective(45, 1, 0.1, 100);
        mat4 transform = res->getTransform();
        setPosition(transform, vec3(0, 0, 5));
        res->self = transform;
        return res;
    }
};

class Scene {
    std::vector<MeshModel*> models;
    std::vector<Light*> lights;
    std::vector<Camera*> cameras;

    void AddModel(MeshModel* model);
    mat4 Projection(); // corrects for nonuniform reshaping

    bool isBloom, isFog, isSupersample, isColorAnim, isVertexAnim, isToon;
    int spreadBloom = 10;
    float threshBloom = 0.5;
    Color colorFog = Color(1);
    float minDistanceFog = 0, maxDistanceFog = 100;
    int factorSupersample = 2;
    int colorAnimType = 0;
    int vertexAnimType = 0;
    int toonShades = 0;
    float firstWidth, firstHeight, width, height;

public:
    int shading = 0;

    int activeModel = -1;
    int activeLight = -1;
    int activeCamera = -1;

    bool dimInactives = true;
    bool drawAxes = false;
    bool drawCameras = false;
    bool drawLights = false;
    bool drawBackshadow = false;

    Scene(int firstWidth, int firstHeight);
    void UpdateDimensions(int newWidth, int newHeight);

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
