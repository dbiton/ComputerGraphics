#pragma once
#include <vector>
#include "CG_skel_w_MFC.h"
#include "vec.h"
#include "mat.h"
#include "GL/glew.h"

using namespace std;

typedef vec3 Color;
// ObjectCoord stored in ModelMesh and does not get used

struct Vertex {
	vec3 coord;
	vec3 normal;
	Color color;
};

class Renderer
{
	float* m_outBuffer; // 3*width*height
	float* m_zbuffer; // width*height
	int m_width, m_height;

	GLuint gScreenTex;
	GLuint gScreenVtc;

	mat4 projection;
	mat4 transform_model;
	mat4 transform_world;
	mat4 transform_camera_inverse;
public:
	Renderer();
	Renderer(int width, int height);
	~Renderer(void);

	void Init();
	// triangles are in object space
	void DrawTriangles(const vector<Vertex>* vertices, const vector<vec3>* face_normals = NULL);
	void SetCameraTransform(const mat4& cTransform);
	void SetProjection(const mat4& projection);
	void SetObjectMatrices(const mat4& oTransform, const mat3& nTransform);
	void SwapBuffers();
	void ClearColorBuffer();
	void ClearDepthBuffer();
	void SetDemoBuffer();

private:
	void DrawLine(const vec2& p0, const Color& c0, const vec2& p1, const Color& c1);
	void DrawPixel(int x, int y, const Color& c);
	void FillTriangle(const vec2& p0, const Color& c0, const vec2& p1, const Color& c1, const vec2& p2, const Color& c2);

	void CreateOpenGLBuffer();
	void InitOpenGLRendering();
	void CreateBuffers(int width, int height);
	void CreateLocalBuffer();
};
