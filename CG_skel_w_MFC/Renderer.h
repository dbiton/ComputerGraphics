#pragma once
#include <vector>
#include "PrimTypes.h"
#include "Material.h"
#include "MeshModel.h"
#include "CG_skel_w_MFC.h"
#include "vec.h"
#include "mat.h"
#include "GL/glew.h"

// ObjectCoord stored in ModelMesh and does not get used

class Light;
class MeshModel;

enum {
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

	GLuint gScreenTex;
	GLuint gScreenVtc;

	mat4 projection;
	mat4 transform_object;
	mat4 transform_camera_inverse;

	std::vector<Light*> lights;
public:
	Renderer();
	Renderer(int width, int height);
	~Renderer(void);

	// triangles are in object space
	void DrawTriangles(MeshModel* model, bool isActiveModel, int shading);
	void DrawBox(const vec3& min, const vec3& max);
	void DrawBox(MeshModel* model);
	void DrawAxes();
	void DrawCamera(const mat4& transform);

	void SetCameraTransform(const mat4& cTransform);
	void SetProjection(const mat4& projection);
	void SetObjectTransform(const mat4& oTransform);

	void SwapBuffers();
	void ClearColorBuffer();
	void ClearDepthBuffer();
	void SetDemoBuffer();
	void CreateBuffers(int width, int height, bool first = false);
	float GetHeightMultiplier() { return m_height / m_firstHeight; }
	float GetWidthMultiplier() { return m_width / m_firstWidth; }

	void setLights(const std::vector<Light*> _lights);
private:
	vec2 clipToScreen(const vec3& clip_pos);
	vec2 clipToScreen(const vec4& clip_pos);
	
	void ShadeTriangle(const vec3 verts[3], const vec3 vert_normals[3], std::vector<Material> mats, int shadeType);
	void DrawLine(const vec2& p0, const vec2& p1, const Color& c);
	void DrawPixel(int x, int y, const Color& c);

	Color CalcColor(const Material& material,
					const vec3& surface_position,
					const vec3& surface_normal);

	void CreateOpenGLBuffer();
	void InitOpenGLRendering();
	void CreateLocalBuffer();
};
