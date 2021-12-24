#include "StdAfx.h"
#include "Renderer.h"
#include "Scene.h"
#include "CG_skel_w_MFC.h"
#include "InitShader.h"
#include "GL\freeglut.h"
#include <set>

#define INDEX(width,x,y,c) (x+y*width)*3+c

Renderer::Renderer()
{
    InitOpenGLRendering();
    CreateBuffers(512, 512, true);
}
Renderer::Renderer(int width, int height)
{
    InitOpenGLRendering();
    CreateBuffers(width, height, true);
}

Renderer::~Renderer(void)
{
}

GLfloat averageLength(vec3 p1, vec3 p2, vec3 p3) {
    return (length(p1 - p2) + length(p1 - p3) + length(p2 - p3)) / 3;
}

void Renderer::DrawTriangles(MeshModel* model, bool isActiveModel, int shading) {
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
    for (int i = 0; i < model->getVertices()->size(); i += 3) {
        vec3 v[3];
        vec3 vn[3];
        vec3 v_clip[3];
        vec3 vn_clip[3];
        vec3 bb_max;
        vec3 bb_min;

        // load vertices
        for (int j = 0; j < 3; j++) {
            v[j] = (*model->getVertices())[i + j].position;
            vn[j] = (*model->getVertices())[i + j].normal;
            v_clip[j] = applyTransformToPoint(object2clip, v[j]);
            vn_clip[j] = applyTransformToPoint(object2clip, vn[j]);
        }

        GLfloat normalsLength = averageLength(v[0], v[1], v[2]);
        vec3 fn = normalize(cross(v[1] - v[0], v[2] - v[1])) * normalsLength * fn_len;; // face normal
        vec3 fm = (v[0] + v[1] + v[2]) / 3; // face mid

        // draw color (shade)
        if (shading != SHADE_NONE) {
            ShadeTriangle(v_clip, vn_clip, model->material->ComputeAt(v, model->getBoundingBoxMin(), model->getBoundingBoxMax()), shading);
        }

        // draw wireframe mesh
        if (model->draw_wireframe) {
            for (int j = 0; j < 3; j++) {
                DrawLine(clipToScreen(v_clip[j]), clipToScreen(v_clip[(j + 1) % 3]), color_mesh);
            }
        }

        // draw face normal
        if (model->draw_normals_per_face) {
            DrawLine(clipToScreen(applyTransformToPoint(object2clip, fm)), clipToScreen(applyTransformToPoint(object2clip, fm + fn)), color_face_normal);
        }

        // draw vertex normals
        if (model->draw_normals_per_vert) {
            for (int j = 0; j < 3; j++) {
                DrawLine(clipToScreen(applyTransformToPoint(object2clip, v[j])),
                    clipToScreen(applyTransformToPoint(object2clip, v[j] + normalize(vn[j]) * normalsLength * vn_len)), color_vert_normal);
            }
        }
    }
}

void Renderer::DrawBox(MeshModel* model) {
    DrawBox(model->getBoundingBoxMin(), model->getBoundingBoxMax());
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
               xminus = 2 * origin - xplus,
               yplus = clipToScreen(applyTransformToPoint(object2clip, vec3(0, 10, 0))),
               yminus = 2 * origin - yplus,
               zplus = clipToScreen(applyTransformToPoint(object2clip, vec3(0, 0, 10))),
               zminus = 2 * origin - zplus;

    // because we don't do clipping, we can't really determine directions at this point
    DrawLine(xminus, xplus, Color(1, 0, 0));
    DrawLine(yminus, yplus, Color(0, 1, 0));
    DrawLine(zminus, zplus, Color(0, 0, 1));
}

mat4& InverseTransform(const mat4& transform) {
    // step 1: isolate translation transform;
    const mat4 translate = Translate(-transform[0][3], -transform[1][3], -transform[2][3]);

    // step 2: find (uniform) scaling factor of the remaining 3x3
    const mat4 scale = Scale(1.0 / std::cbrt(Determinant(transform)));

    // step 3: transpose remaining matrix
    const mat4 rotation = transpose(scale * translate * transform);

    return rotation * scale * translate; // final product
}


void Renderer::DrawCamera(const mat4& transform) {
    const mat4 object2clip = projection * transform_camera_inverse * transform;

    const vec4 pos4 = getPosition(transform);
    const vec3 pos3(pos4.x / pos4.w, pos4.y / pos4.w, pos4.z / pos4.w);
    const vec2 pos2 = clipToScreen(applyTransformToPoint(object2clip, pos3));

    const GLfloat len_axis = 1.f;

    const vec2 side2 = clipToScreen(applyTransform(object2clip, (pos3 + len_axis * vec3(1, 0, 0))));
    const vec2 up2 = clipToScreen(applyTransform(object2clip, (pos3 + len_axis * vec3(0, 1, 0))));
    const vec2 forward2 = clipToScreen(applyTransform(object2clip, (pos3 + len_axis * vec3(0, 0, 1))));

    DrawLine(pos2, side2, Color(0, 1, 1));
    DrawLine(pos2, up2, Color(1, 0, 1));
    DrawLine(pos2, forward2, Color(1, 1, 0));
}

void Renderer::SetCameraTransform(const mat4& cTransform)
{
    transform_camera_inverse = InverseTransform(cTransform);
}

void Renderer::SetProjection(const mat4& _projection)
{
    projection = _projection;
}

void Renderer::SetObjectTransform(const mat4& oTransform)
{
    transform_object = oTransform;
}

void Renderer::CreateBuffers(int width, int height, bool first)
{
    m_width = width;
    m_height = height;
    if (first) {
        m_outBuffer = nullptr;
        m_zbuffer = nullptr;
        m_firstWidth = m_width;
        m_firstHeight = m_height;
    }
    CreateOpenGLBuffer(); //Do not remove this line.
    if (m_outBuffer) delete m_outBuffer;
    m_outBuffer = new float[3 * m_width * m_height];
    if (m_zbuffer) delete m_zbuffer;
    m_zbuffer = new float[m_width * m_height];
}

vec2 Renderer::clipToScreen(const vec4& clip_pos)
{
    return clipToScreen(vec3(clip_pos.x / clip_pos.w, clip_pos.y / clip_pos.w, clip_pos.z / clip_pos.w));
}


void Renderer::setLights(const std::vector<Light*> _lights)
{
    lights = _lights;
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

constexpr float EPSILON_INVERSE = 1e7;

bool bad(float f) noexcept {
    return !_finite(f) || std::abs(f) > EPSILON_INVERSE;
}

float Area(vec2 p0, vec2 p1, vec2 p2) noexcept
{
    const vec2 p1_p0 = p1 - p0,
               p2_p0 = p2 - p0;
    return 0.5f * std::abs(p1_p0.x * p2_p0.y - p2_p0.x * p1_p0.y);
}

void Renderer::ShadeTriangle(const vec3 v[3], const vec3 vn[3], std::vector<Material> mats, int shading){
    const vec3 fm = (v[0] + v[1] + v[2]) / 3;
    const vec3 fn = normalize(cross(v[1] - v[0], v[2] - v[1]));
    const vec3 camera_dir(0, 0, 1);
    
    // backface culling
    if (dot(fn, camera_dir) < 0) return;

    const vec2 v2d[3] = { clipToScreen(v[0]), clipToScreen(v[1]), clipToScreen(v[2]) };
    const float A = Area(v2d[0], v2d[1], v2d[2]);
    if (A < FLT_EPSILON) return; // triangle is a line
    
    // precalculate colors if need be
    vec3 color_face;
    vec3 color_vert[3];
    if (shading == SHADE_FLAT)
        color_face = CalcColor(AverageMaterial(mats), (v[0] + v[1] + v[2]) / 3, fn);
    
    else if (shading == SHADE_GOURAUD)
        for (int i = 0; i < 3; i++) color_vert[i] = CalcColor(mats[i], v[i], vn[i]);
    
    std::set<int> visited_pixels;
    std::vector<vec2> pixels;
    vec2 v2d_mid = (v2d[0] + v2d[1] + v2d[2]) / 3;
    v2d_mid.x = std::round(v2d_mid.x);
    v2d_mid.y = std::round(v2d_mid.y);
    pixels.push_back(v2d_mid);
    while (pixels.size() > 0) {
        const vec2 p = pixels.back();
        pixels.pop_back();
        visited_pixels.insert(p.x + p.y * m_width);
        // out of frame

        const float a0 = Area(p, v2d[1], v2d[2]) / A,
                    a1 = Area(p, v2d[2], v2d[0]) / A,
                    a2 = Area(p, v2d[0], v2d[1]) / A;
        
        if (std::abs(a0 + a1 + a2 - 1) > FLT_EPSILON) continue; // If out of the triangle
        
        const int x = p.x,
                  y = p.y; // minimize float operations...
        if (x > m_width || x < 0 || y > m_height || y < 0) continue;

        const float depth = a0 * v[0].z + a1 * v[1].z + a2 * v[2].z;
        if (depth > m_zbuffer[x + y * m_width]) {
            m_zbuffer[x + y * m_width] = depth;
            Color color;
            switch (shading) {
            case SHADE_FLAT:
                color = color_face;
                break;
            case SHADE_GOURAUD:
                color = a0 * color_vert[0] + a1 * color_vert[1] + a2 * color_vert[2];
                break;
            case SHADE_PHONG:
                color = CalcColor(InterpolateMaterial(mats, a0, a1, a2), a0 * v[0] + a1 * v[1] + a2 * v[2], a0 * vn[0] + a1 * vn[1] + a2 * vn[2]);
                break;
            default:
                printf("ShadeType unimplemented!");
                exit(1);
            }
            DrawPixel(p.x, p.y, color);
        }
        vec2 p_next[4] = { p + vec2(1,  0), p + vec2(-1,  0), p + vec2(0,  1), p + vec2(0, -1) };
        for (int i = 0; i < 4; i++) {
            const int x = p_next[i].x,
                      y = p_next[i].y;
            if (visited_pixels.count(x + y * m_width) == 0) {
                visited_pixels.insert(x + y * m_width);
                pixels.push_back(p_next[i]);
            }
        }
    }
}

void Renderer::DrawLine(const vec2& p0, const vec2& p1, const Color& c)
{
    if (bad(p0.x) || bad(p0.y) || bad(p1.x) || bad(p1.y)) return; // do nothing!

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

Color Renderer::CalcColor(const Material& material, const vec3& surface_position, const vec3& surface_normal)
{
    Color color = material.emissive;
    for (const auto& light : lights) {
        const vec3 l = light->dirToSource(surface_position);

        // this is an ambient light source
        if (length(l) == 0) {
            // ambient
            const vec3 alc = material.ambient_reflect * light->getBrightness() * light->getColor();
            color += alc * material.base;
        }
        else {
            const vec3 v = normalize(getPosition(transform_camera_inverse) - surface_position);
            const vec3 r = l - 2 * cross(dot(l, surface_normal), surface_normal);

            // diffuse
            const vec3 dlc = material.roughness * dot(l, surface_normal) * light->getBrightness() * light->getColor();
            color += dlc * material.base;

            // specular
            const vec3 slc = (1 - material.roughness) * pow(abs(dot(r, v)), material.shininess) * light->getBrightness() * light->getColor();
            color += slc * material.base;
        }
    }
    // color values should be in the [0,1] range, so we bound them using x/(1+x)
    for (int i = 0; i < 3; i++) color[i] /= (1 + color[i]);
    return color;
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
    if (m_zbuffer) memset(m_zbuffer, FLT_MAX, sizeof(float) * m_width * m_height);
}
