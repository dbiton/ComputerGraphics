#include "StdAfx.h"
#include "Renderer.h"
#include "CG_skel_w_MFC.h"
#include "InitShader.h"
#include "GL\freeglut.h"

#define INDEX(width,x,y,c) (x+y*width)*3+c

Renderer::Renderer() : m_width(512), m_height(512), m_outBuffer(nullptr), m_zbuffer(nullptr)
{
    InitOpenGLRendering();
    CreateBuffers(512, 512);
}
Renderer::Renderer(int width, int height) : m_width(width), m_height(height), m_outBuffer(nullptr), m_zbuffer(nullptr)
{
    InitOpenGLRendering();
    CreateBuffers(width, height);
}

Renderer::~Renderer(void)
{
}

GLfloat averageLength(vec3 p1, vec3 p2, vec3 p3) {
    return (length(p1 - p2) + length(p1 - p3) + length(p2 - p3)) / 3;
}

void Renderer::DrawTriangles(const vector<Vertex>& vertices, bool isActiveModel, bool drawFaceNormals, bool drawVertexNormals)
{
    Color color_mesh(1, 1, 1);
    Color color_face_normal(0, 1, 0);
    Color color_vert_normal(0, 0.5, 1);
    if (!isActiveModel) {
        const float fade = 0.5;
        color_mesh *= fade;
        color_face_normal *= fade;
        color_vert_normal *= fade;
    }
    GLfloat fn_len = 0.25;
    GLfloat vn_len = 0.5;

    mat4 object2clip = projection * transform_camera_inverse * transform_object;
    for (int i = 0; i < vertices.size(); i += 3) {
        vec3 v[3];
        vec3 vn[3];
        vec3 fn; // face normal
        vec3 fm; // face mid
        vec3 bb_max;
        vec3 bb_min;

        // load vertices
        for (int j = 0; j < 3; j++) {
            v[j] = vertices[i + j].position;
            vn[j] = vertices[i + j].normal;
        }
        // draw mesh
        for (int j = 0; j < 3; j++) {
            DrawLine(clipToScreen(applyTransformToPoint(object2clip, v[j])), clipToScreen(applyTransformToPoint(object2clip, v[(j + 1) % 3])), color_mesh);
        }

        GLfloat normalsLength = averageLength(v[0], v[1], v[2]);

        // draw face normal
        if (drawFaceNormals) {
            fm = (v[0] + v[1] + v[2]) / 3;
            fn = normalize(cross(v[1] - v[0], v[2] - v[1])) * normalsLength * fn_len;
            DrawLine(clipToScreen(applyTransformToPoint(object2clip, fm)), clipToScreen(applyTransformToPoint(object2clip, fm + fn)), color_face_normal);
        }

        // draw vertex normals
        if (drawVertexNormals) {
            for (int j = 0; j < 3; j++) {
                DrawLine(clipToScreen(applyTransformToPoint(object2clip, v[j])), clipToScreen(applyTransformToPoint(object2clip, v[j] + normalize(vn[j]) * normalsLength * vn_len)), color_vert_normal);
            }
        }
    }
}

void Renderer::DrawBox(const vec3& min, const vec3& max) {
    const mat4 object2clip = projection * transform_camera_inverse * transform_object;

    const vec2 v000 = clipToScreen(applyTransformToPoint(object2clip, min)),
               v100 = clipToScreen(applyTransformToPoint(object2clip, vec3(max.x, min.y, min.z))),
               v010 = clipToScreen(applyTransformToPoint(object2clip, vec3(min.x, max.y, min.z))),
               v110 = clipToScreen(applyTransformToPoint(object2clip, vec3(max.x, max.y, min.z))),
               v001 = clipToScreen(applyTransformToPoint(object2clip, vec3(min.x, min.y, max.z))),
               v101 = clipToScreen(applyTransformToPoint(object2clip, vec3(max.x, min.y, max.z))),
               v011 = clipToScreen(applyTransformToPoint(object2clip, vec3(min.x, max.y, max.z))),
               v111 = clipToScreen(applyTransformToPoint(object2clip, max));
    const Color color_bounding_box(1, 0, 0);

    DrawLine(v000, v100, color_bounding_box);
    DrawLine(v000, v010, color_bounding_box);
    DrawLine(v000, v001, color_bounding_box);

    DrawLine(v110, v100, color_bounding_box);
    DrawLine(v101, v100, color_bounding_box);
    DrawLine(v110, v010, color_bounding_box);
    DrawLine(v011, v010, color_bounding_box);
    DrawLine(v101, v001, color_bounding_box);
    DrawLine(v011, v001, color_bounding_box);

    DrawLine(v111, v011, color_bounding_box);
    DrawLine(v111, v101, color_bounding_box);
    DrawLine(v111, v110, color_bounding_box);
}

void Renderer::DrawAxes() {
    const mat4 object2clip = projection * transform_camera_inverse; // axes belong to the world, so no transform_object!
    const vec2 origin = clipToScreen(applyTransformToPoint(object2clip, vec3(0, 0, 0))),
               xplus = clipToScreen(applyTransformToPoint(object2clip, vec3(10, 0, 0))),
               xminus = clipToScreen(applyTransformToPoint(object2clip, vec3(-10, 0, 0))),
               yplus = clipToScreen(applyTransformToPoint(object2clip, vec3(0, 10, 0))),
               yminus = clipToScreen(applyTransformToPoint(object2clip, vec3(0, -10, 0))),
               zplus = clipToScreen(applyTransformToPoint(object2clip, vec3(0, 0, 10))),
               zminus = clipToScreen(applyTransformToPoint(object2clip, vec3(0, 0, -10)));

    DrawLine(origin, xplus, Color(1, 0, 0));
    DrawLine(origin, xminus, Color(0.5, 0, 0));
    DrawLine(origin, yplus, Color(0, 1, 0));
    DrawLine(origin, yminus, Color(0, 0.5, 0));
    DrawLine(origin, zplus, Color(0, 0, 1));
    DrawLine(origin, zminus, Color(0, 0, 0.5));
}

void Renderer::SetCameraTransform(const mat4& cTransform)
{
    transform_camera_inverse = transpose(cTransform);
}

void Renderer::SetProjection(const mat4& _projection)
{
    projection = _projection;
}

void Renderer::SetObjectTransform(const mat4& oTransform)
{
    transform_object = oTransform;
}

void Renderer::CreateBuffers(int width, int height)
{
    m_width = width;
    m_height = height;
    CreateOpenGLBuffer(); //Do not remove this line.
    if (m_outBuffer) delete m_outBuffer;
    m_outBuffer = new float[3 * m_width * m_height];
}

vec2 Renderer::clipToScreen(const vec3& clip_pos)
{
    return vec2((clip_pos.x + 1) / 2 * m_width, (clip_pos.y + 1) / 2 * m_height);
}

void Renderer::CreateLocalBuffer()
{
}

void Renderer::SetDemoBuffer()
{
    const int halfWidth = m_width / 2,
              halfHeight = m_height / 2;
    DrawLine(vec2(halfWidth, 0), vec2(halfWidth, m_height), Color(1, 0, 0));
    DrawLine(vec2(0, halfHeight), vec2(m_width, halfHeight), Color(1, 0, 1));
    constexpr bool TEST_RAYS = true;
    if (TEST_RAYS) {
        constexpr int rays = 2000;
        constexpr float fraction = (2.0 / rays) * M_PI;
        for (int i = 0; i <= rays; i++)
        {
            DrawLine(vec2(halfWidth, halfHeight), vec2(halfWidth + 200 * cos(fraction * i), halfHeight + 200 * sin(fraction * i)), Color(1, 1, 1));
        }
    }
}

inline static constexpr int sign(int n) noexcept { return (n > 0) ? 1 : (n < 0) ? -1 : 0; }

void Renderer::DrawLine(const vec2& p0, const vec2& p1, const Color& c)
{
    const int x0 = std::round(p0.x),
              y0 = std::round(p0.y),
              x1 = std::round(p1.x),
              y1 = std::round(p1.y), // that's it! no more floats! only ints from here on out!
              dx = x1 - x0,
              dy = y1 - y0,
              dy2 = 2 * abs(dy),
              dx2 = 2 * abs(dx),
              x_step = sign(dx),
              y_step = sign(dy);
    int d = 0,
        x = x0,
        y = y0;

    // draw first+mid pixels
    if (dy2 < dx2) while (x != x1) {
        DrawPixel(x, y, c);
        x += x_step;
        if (d >= 0) { y += y_step; d -= dx2; }
        d += dy2;
    }
    else while (y != y1) {
        DrawPixel(x, y, c);
        y += y_step;
        if (d >= 0) { x += x_step; d -= dy2; }
        d += dx2;
    }

    // draw last pixel
    DrawPixel(x1, y1, c);
}

void Renderer::DrawPixel(int x, int y, const Color& c)
{
    if (x < 0 || y < 0 || x >= m_width || y >= m_height) return;
    m_outBuffer[INDEX(m_width, x, y, 0)] = c[0];
    m_outBuffer[INDEX(m_width, x, y, 1)] = c[1];
    m_outBuffer[INDEX(m_width, x, y, 2)] = c[2];
}


/////////////////////////////////////////////////////
//OpenGL stuff. Don't touch.

void Renderer::InitOpenGLRendering()
{
    int a = glGetError();
    a = glGetError();
    glGenTextures(1, &gScreenTex);
    a = glGetError();
    glGenVertexArrays(1, &gScreenVtc);
    GLuint buffer;
    glBindVertexArray(gScreenVtc);
    glGenBuffers(1, &buffer);
    const GLfloat vtc[] = {
        -1, -1,
        1, -1,
        -1, 1,
        -1, 1,
        1, -1,
        1, 1
    };
    const GLfloat tex[] = {
        0,0,
        1,0,
        0,1,
        0,1,
        1,0,
        1,1 };
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vtc) + sizeof(tex), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

    GLuint program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);
    GLint  vPosition = glGetAttribLocation(program, "vPosition");

    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0,
        0);

    GLint  vTexCoord = glGetAttribLocation(program, "vTexCoord");
    glEnableVertexAttribArray(vTexCoord);
    glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
        (GLvoid*)sizeof(vtc));
    glProgramUniform1i(program, glGetUniformLocation(program, "texture"), 0);
    a = glGetError();
}

void Renderer::CreateOpenGLBuffer()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gScreenTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_width, m_height, 0, GL_RGB, GL_FLOAT, NULL);
    glViewport(0, 0, m_width, m_height);
}

void Renderer::SwapBuffers()
{

    int a = glGetError();
    glActiveTexture(GL_TEXTURE0);
    a = glGetError();
    glBindTexture(GL_TEXTURE_2D, gScreenTex);
    a = glGetError();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_FLOAT, m_outBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);
    a = glGetError();

    glBindVertexArray(gScreenVtc);
    a = glGetError();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    a = glGetError();
    glutSwapBuffers();
    a = glGetError();
}

void Renderer::ClearColorBuffer()
{
    if (m_outBuffer) memset(m_outBuffer, 0, sizeof(float) * 3 * m_width * m_height);
}

void Renderer::ClearDepthBuffer()
{
    if (m_zbuffer) memset(m_zbuffer, 0, sizeof(float) * m_width * m_height);
}
