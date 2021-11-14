#pragma once
#include <vector>
#include "PrimTypes.h"
#include "MeshModel.h"
#include "CG_skel_w_MFC.h"
#include "vec.h"
#include "mat.h"
#include "GL/glew.h"

using namespace std;

// ObjectCoord stored in ModelMesh and does not get used

class Renderer
{
	float* m_outBuffer; // 3*width*height
	float* m_zbuffer; // width*height
	int m_width, m_height;

	GLuint gScreenTex;
	GLuint gScreenVtc;

	mat4 projection;
	mat4 transform_object;
	mat4 transform_camera_inverse;

public:
	Renderer();
	Renderer(int width, int height);
	~Renderer(void);

	// triangles are in object space
	void DrawTriangles(const std::vector<Vertex>& vertices, bool drawFaceNormals, bool drawVertexNormals);
	void DrawBox(const vec3& min, const vec3& max);

	void SetCameraTransform(const mat4& cTransform);
	void SetProjection(const mat4& projection);
	void SetObjectTransform(const mat4& oTransform);

	void SwapBuffers();
	void ClearColorBuffer();
	void ClearDepthBuffer();
	void SetDemoBuffer();
	void CreateBuffers(int width, int height);
private:
	vec2 clipToScreen(const vec3& clip_pos);

	void DrawLine(const vec2& p0, const vec2& p1, const Color& c);
	void DrawPixel(int x, int y, const Color& c);

	void CreateOpenGLBuffer();
	void InitOpenGLRendering();
	void CreateLocalBuffer();
};
