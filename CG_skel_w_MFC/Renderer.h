#pragma once
#include <vector>
#include "GL/glew.h"
#include "mat.h"
#include "Material.h"
#include "MeshModel.h"
#include "PrimTypes.h"
#include "Resource.h"
#include "vec.h"

// ObjectCoord stored in ModelMesh and does not get used

class Light;
class MeshModel;

enum ShadeType {
	SHADE_NONE,
	SHADE_FLAT,
	SHADE_GOURAUD,
	SHADE_PHONG
};

class Renderer
{
	float* m_outBuffer; // 3*width*height
	float* m_zbuffer; // width*height
	int m_width, m_height;
	float m_firstWidth, m_firstHeight;

	bool m_isSuperSample = false;
	int m_factorSuperSample = 3;
	float* m_outBufferSuperSample; // 3*width*height
	float* m_zbufferSuperSample; // width*height

	bool m_isFog = false;
	Color m_fogColor = Color();
	float m_fogMaxDistance = 0;
	float m_fogMinDistance = 0;

	bool m_isBloom = false;
	float* m_bloomBufferHorz, *m_bloomBufferVert;
	float* m_weightsBloom;
	float m_threshBloom = 1;
	int m_spreadBloom = 0;

	GLuint gScreenTex;
	GLuint gScreenVtc;

	mat4 projection;
	mat4 transform_object;
	mat4 transform_camera, transform_camera_inverse;

	vec3 camera_pos, model_pos;

	std::vector<Light*> lights;
	int ambientLightDrawn = 0;
public:
	bool drawBackshadow = false;

	Renderer();
	Renderer(int width, int height);
	~Renderer();

	// triangles are in object space
	void DrawTriangles(MeshModel* model, bool isActiveModel, int shading);
	void DrawBox(const vec3& min, const vec3& max);
	void DrawBox(MeshModel* model);
	void DrawAxes();
	void DrawCamera(const mat4& transform);
	void DrawLightsStart();
	void DrawLight(Light* light, bool isActiveLight);

	void SetCameraTransform(const mat4& cTransform);
	void SetProjection(const mat4& projection);
	void SetObjectTransform(const mat4& oTransform);

	void SwapBuffers();
	void ClearColorBuffer();
	void ClearDepthBuffer();
	void ClearBloomBuffer();
	void SetDemoBuffer();
	void CreateBuffers(int width, int height);
	void ResizeBuffers(int width, int height);
	float GetHeightMultiplier() noexcept { return m_height / m_firstHeight; }
	float GetWidthMultiplier() noexcept { return m_width / m_firstWidth; }

	void setLights(const std::vector<Light*> _lights);

	void setSupersampling(bool isSuperSample, int factorSuperSample = 3);
	void setFog(bool isFog, Color fogColor = vec3(), float fogMinDistance = 0, float fogMaxDistance = 0);
	void setBloom(bool isBloom, float threshBloom = 1.f, int spreadBloom = 0);
	int getSupersamplingFactor() noexcept { return m_factorSuperSample; }
	Color getFogColor() noexcept { return m_fogColor; }
	float getFogMaxDistance() noexcept { return m_fogMaxDistance; }
	float getFogMinDistance() noexcept { return m_fogMinDistance; }
	float getThreshBloom() noexcept { return m_threshBloom; }
	int getSpreadBloom() noexcept { return m_spreadBloom; }

	void applyEffects();
private:
	void blendBloomBuffer();

	vec2 clipToScreen(const vec3& clip_pos);
	vec2 clipToScreen(const vec4& clip_pos);
	
	void ShadeTriangle(const vec3 verts[3], const vec3 vert_normals[3], std::vector<Material> mats, int shadeType, const mat4& world2clip);
	void DrawLine(vec2 p0, vec2 p1, const Color& c);
	void DrawPixel(int x, int y, const Color& c);
	void DrawPixelSuperSampled(int x, int y, const Color& c);

	Color CalcColor(const Material& material,
					const vec3& surface_position,
					const vec3& surface_normal);

	void CreateOpenGLBuffer();
	void InitOpenGLRendering();
	void CreateLocalBuffer();
};
