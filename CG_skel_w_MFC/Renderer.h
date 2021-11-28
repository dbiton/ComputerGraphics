#pragma once
#include <vector>
#include "PrimTypes.h"
#include "Material.h"
#include "MeshModel.h"
#include "CG_skel_w_MFC.h"
#include "vec.h"
#include "mat.h"
#include "GL/glew.h"

using namespace std;

// ObjectCoord stored in ModelMesh and does not get used

class Camera;
class Material;
class Light;

enum ShadeType{
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

	const std::vector<Light*>* lights;
public:
	Renderer();
	Renderer(int width, int height);
	~Renderer(void);

	// triangles are in object space
	void DrawTriangles(
		const std::vector<Vertex>* vertices, 
		bool isActiveModel, 
		bool drawFaceNormals, 
		bool drawVertexNormals,
		bool drawWireframe = true,
		ShadeType shadeType = SHADE_NONE,
		Material material = Material::DefaultMaterial());
	void DrawBox(const vec3& min, const vec3& max);
	void DrawAxes();
	void DrawCamera(const Camera* camera);

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

	void setLights(const std::vector<Light*>* _lights);
private:
	vec2 clipToScreen(const vec3& clip_pos);
	vec2 clipToScreen(const vec4& clip_pos);
	
	void ShadeTriangle(vec3 p3d[3], Material material, ShadeType shadeType);
	void DrawLine(const vec2& p0, const vec2& p1, const Color& c);
	void DrawPixel(int x, int y, const Color& c);

	Color CalcColor(const Material& material,
					const vec3& surface_position,
					const vec3& surface_normal);

	float Area(vec2 p0, vec2 p1, vec2 p2);
	bool PixelInsideTriangle(vec2 pixel, vec2 p2d[3]);

	void CreateOpenGLBuffer();
	void InitOpenGLRendering();
	void CreateLocalBuffer();
};
