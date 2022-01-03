#include "stdafx.h"

#include <set>
#include "GL\glew.h"
#include "GL\freeglut.h"
#include "InitShader.h"
#include "Renderer.h"
#include "Resource.h"
#include "Scene.h"

#define INDEX(width,x,y,c) (x+y*width)*3+c

Renderer::Renderer() {
    InitOpenGLRendering();
    CreateBuffers(512, 512, true);
}

Renderer::Renderer(int width, int height) {
    InitOpenGLRendering();
    CreateBuffers(width, height, true);
}

Renderer::~Renderer() { }

GLfloat averageLength(vec3 p1, vec3 p2, vec3 p3) {
    return (length(p1 - p2) + length(p1 - p3) + length(p2 - p3)) / 3;
}

void Renderer::DrawTriangles(MeshModel* model, bool isActiveModel, int shading) {
    Color color_mesh(1, 1, 1), 
          color_face_normal(0, 1, 0),
          color_vert_normal(0, 0.5, 1);
    if (!isActiveModel) {
        constexpr float fade = 0.5;
        color_mesh *= fade;
        color_face_normal *= fade;
        color_vert_normal *= fade;
    }
    constexpr GLfloat fn_len = 0.25,
                      vn_len = 0.5;

    const mat4 world2clip = projection * transform_camera_inverse;
    for (int i = 0; i < model->getVertices()->size(); i += 3) {
        vec3 v[3];
        vec3 vn[3];
        vec2 v_screen[3];

        // load vertices
        for (int j = 0; j < 3; j++) {
            v[j] = applyTransformToPoint(transform_object, (*model->getVertices())[i + j].position);
            vn[j] = applyTransformToNormal(transform_object, (*model->getVertices())[i + j].normal);
            if (model->draw_wireframe) v_screen[j] = clipToScreen(applyTransformToPoint(world2clip, v[j]));
        }

        // draw color (shade)
        if (shading != SHADE_NONE) {
            ShadeTriangle(v, vn, model->material->ComputeAt(v, model->getBoundingBoxMin(), model->getBoundingBoxMax()), shading, projection * transform_camera_inverse);
        }

        // draw wireframe mesh
        if (model->draw_wireframe) {
            for (int j = 0; j < 3; j++) {
                DrawLine(v_screen[j], v_screen[(j + 1) % 3], color_mesh);
            }
        }

        const GLfloat normalsLength = averageLength(v[0], v[1], v[2]);

        // draw face normal
        if (model->draw_normals_per_face) {
            const vec3 fn = normalize(cross(v[1] - v[0], v[2] - v[1])) * normalsLength * fn_len, // face normal
                       fm = (v[0] + v[1] + v[2]) / 3; // face mid
            DrawLine(clipToScreen(applyTransformToPoint(world2clip, fm)), clipToScreen(applyTransformToPoint(world2clip, fm + fn)), color_face_normal);
        }

        // draw vertex normals
        if (model->draw_normals_per_vert) {
            for (int j = 0; j < 3; j++) {
                DrawLine(clipToScreen(applyTransformToPoint(world2clip, v[j])),
                    clipToScreen(applyTransformToPoint(world2clip, v[j] + normalize(vn[j]) * normalsLength * vn_len)), color_vert_normal);
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

void Renderer::DrawLightsStart() {
    ambientLightDrawn = 0;
}

void Renderer::DrawLight(Light* light, bool isActiveLight) {
    Color color = light->getColor() * light->getBrightness();
    for (int i = 0; i < 3; i++) color[i] /= (1 + color[i]); // normalize...
    
    switch (light->getType()) {
    case LIGHT_AMBIENT: {
        const vec2 boxMin(10 + ambientLightDrawn++ * 20, 10);

        for (int i = 0; i < 10; i++)
            DrawLine(boxMin + vec2(i, 0), boxMin + vec2(i, 10), color);
    } break;
    case LIGHT_POINT: {
        const vec2 pos = clipToScreen(applyTransformToPoint(projection * transform_camera_inverse, ((PointLight*)light)->getPosition())),
                   crossMin = pos - vec2(5),
                   crossMax = pos + vec2(5);

        DrawLine(crossMin, crossMax, color);
        DrawLine(vec2(crossMin.x, crossMax.y), vec2(crossMax.x, crossMin.y), color);
    } break;
    case LIGHT_PARALLEL: {
        const vec2 dir = clipToScreen(applyTransformToNormal(projection * transform_camera_inverse, ((ParallelLight*)light)->getDirection())),
            center = vec2(m_width, m_height) / 2,
            lineMax = center + (m_height / 2) * normalize(dir),
            offset = 10 * normalize(vec2(-dir.y, dir.x));

        DrawLine(center, lineMax, color);
        DrawLine(center + offset, lineMax + offset, color);
        DrawLine(center - offset, lineMax - offset, color);
    } break;
    default:
        printf("LightType unimplemented!");
        exit(1);
    }
}

void Renderer::SetCameraTransform(const mat4& cTransform) {
    transform_camera_inverse = InverseTransform(cTransform);
}

void Renderer::SetProjection(const mat4& _projection) {
    projection = _projection;
}

void Renderer::SetObjectTransform(const mat4& oTransform) {
    transform_object = oTransform;
}

void Renderer::CreateBuffers(int width, int height, bool first) {
    m_width = width;
    m_height = height;
    if (first) {
        m_weightsBloom = nullptr;
        m_outBufferSuperSample = nullptr;
        m_bloomBuffer = nullptr;
        m_outBuffer = nullptr;
        m_zbuffer = nullptr;
        m_zbufferSuperSample = nullptr;
        m_firstWidth = m_width;
        m_firstHeight = m_height;
    }
    CreateOpenGLBuffer(); //Do not remove this line.
    if (m_outBuffer) delete m_outBuffer;
    m_outBuffer = new float[3 * m_width * m_height];
    if (m_zbuffer) delete m_zbuffer;
    m_zbuffer = new float[m_width * m_height];
}

vec2 Renderer::clipToScreen(const vec4& clip_pos) {
    return clipToScreen(vec3(clip_pos.x / clip_pos.w, clip_pos.y / clip_pos.w, clip_pos.z / clip_pos.w));
}

void Renderer::setLights(const std::vector<Light*> _lights) {
    lights = _lights;
}

vec2 Renderer::clipToScreen(const vec3& clip_pos) {
    return vec2((clip_pos.x + 1) / 2 * m_width, (clip_pos.y + 1) / 2 * m_height);
}

void Renderer::CreateLocalBuffer() { }

void Renderer::SetDemoBuffer() {
    const int halfWidth = m_width / 2,
              halfHeight = m_height / 2;
    DrawLine(vec2(halfWidth, 0), vec2(halfWidth, m_height), Color(1, 0, 0));
    DrawLine(vec2(0, halfHeight), vec2(m_width, halfHeight), Color(1, 0, 1));
    constexpr bool TEST_RAYS = true;
    if (TEST_RAYS) {
        constexpr int rays = 2000;
        constexpr float fraction = (2.0 / rays) * M_PI;
        for (int i = 0; i <= rays; i++) {
            DrawLine(vec2(halfWidth, halfHeight), vec2(halfWidth + 200 * cos(fraction * i), halfHeight + 200 * sin(fraction * i)), Color(1, 1, 1));
        }
    }
}

inline static constexpr int sign(int n) noexcept { return (n > 0) ? 1 : (n < 0) ? -1 : 0; }

constexpr float EPSILON_INVERSE = 1e7;

bool bad(float f) noexcept {
    return !_finite(f) || std::abs(f) > EPSILON_INVERSE;
}

float Area(vec2 p0, vec2 p1, vec2 p2) noexcept {
    const vec2 p1_p0 = p1 - p0,
               p2_p0 = p2 - p0;
    return 0.5f * std::abs(p1_p0.x * p2_p0.y - p2_p0.x * p1_p0.y);
}

void Renderer::ShadeTriangle(const vec3 v[3], const vec3 vn[3], std::vector<Material> mats, int shading, const mat4& world2clip) {
    vec3 v_clip[3] = { applyTransformToPoint(world2clip, v[0]), applyTransformToPoint(world2clip, v[1]), applyTransformToPoint(world2clip, v[2]) };

    // backface culling
    if (dot(cross(v_clip[1] - v_clip[0], v_clip[2] - v_clip[1]), vec3(0, 0, -1)) < 0) return;

    vec2 v2d[3] = { clipToScreen(v_clip[0]), clipToScreen(v_clip[1]), clipToScreen(v_clip[2]) };
    int width = m_width,
        height = m_height;
    if (m_isSuperSample) {
        for (int i = 0; i < 3; i++) v2d[i] *= m_factorSuperSample;
        width *= m_factorSuperSample;
        height *= m_factorSuperSample;
    }

    const float A = Area(v2d[0], v2d[1], v2d[2]);
    if (A < FLT_EPSILON) return; // triangle is a line

    // precalculate colors if need be
    vec3 color_face;
    vec3 color_vert[3];
    if (shading == SHADE_FLAT)
        color_face = CalcColor(AverageMaterial(mats), (v[0] + v[1] + v[2]) / 3, normalize(cross(v[1] - v[0], v[2] - v[1])));

    else if (shading == SHADE_GOURAUD)
        for (int i = 0; i < 3; i++) color_vert[i] = CalcColor(mats[i], v[i], vn[i]);

    int x_min = width - 1, x_max = 0, y_min = height - 1, y_max = 0;
    for (int i = 0; i < 3; i++) {
        x_min = min(x_min, v2d[i].x);
        y_min = min(y_min, v2d[i].y);
        x_max = max(x_max, v2d[i].x);
        y_max = max(y_max, v2d[i].y);
    }
    x_min = max(0, x_min);
    x_max = min(width - 1, x_max);
    y_min = max(0, y_min);
    y_max = min(height - 1, y_max);

    for (int x = x_min; x <= x_max; x++) {
        for (int y = y_min; y <= y_max; y++) {
            const vec2 p(x, y);
            // out of frame

            const float a0 = Area(p, v2d[1], v2d[2]) / A,
                        a1 = Area(p, v2d[2], v2d[0]) / A,
                        a2 = Area(p, v2d[0], v2d[1]) / A;

            if (std::abs(a0 + a1 + a2 - 1) > 16 * FLT_EPSILON) continue; // If out of the triangle

            const float depth = a0 * v_clip[0].z + a1 * v_clip[1].z + a2 * v_clip[2].z;

            bool depth_updated = false;
            if (m_isSuperSample) {
                if (m_zbufferSuperSample[x + y * width] == 0 || depth < m_zbufferSuperSample[x + y * width]) {
                    m_zbufferSuperSample[x + y * width] = depth;
                    depth_updated = true;
                }
            }
            else if (m_zbuffer[x + y * width] == 0 || depth < m_zbuffer[x + y * width]) {
                m_zbuffer[x + y * width] = depth;
                depth_updated = true;
            }

            if (depth_updated) {
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
                if (m_isFog) {
                    const vec3 v_camera[3] = { applyTransformToPoint(transform_camera_inverse, v[0]),
                        applyTransformToPoint(transform_camera_inverse, v[1]),
                        applyTransformToPoint(transform_camera_inverse, v[2]) };
                    const float depth2 = abs(a0 * v_camera[0].z + a1 * v_camera[1].z + a2 * v_camera[2].z);
                    float fogFactor = (m_fogMaxDistance - depth2) / (m_fogMaxDistance - m_fogMinDistance);
                    fogFactor = max(0.f, min(1.f, fogFactor)); // clamp
                    color = (1-fogFactor) * m_fogColor + fogFactor * color;
                }
                if (m_isSuperSample) {
                    DrawPixelSuperSampled(p.x, p.y, color);
                }
                else {
                    DrawPixel(p.x, p.y, color);
                }
            }
        }
    }
}

void Renderer::DrawLine(vec2 p0, vec2 p1, const Color& c) {
    if (bad(p0.x) || bad(p0.y) || bad(p1.x) || bad(p1.y)) return; // do nothing!

    if (m_isSuperSample) {
        p0 *= m_factorSuperSample;
        p1 *= m_factorSuperSample;
    }
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
        if (m_isSuperSample) {
            DrawPixelSuperSampled(x, y, c);
        }
        else {
            DrawPixel(x, y, c);
        }
        x += x_step;
        if (d >= 0) { y += y_step; d -= dx2; }
        d += dy2;
    }
    else while (y != y1) {
        if (m_isSuperSample) {
            DrawPixelSuperSampled(x, y, c);
        }
        else {
            DrawPixel(x, y, c);
        }
        y += y_step;
        if (d >= 0) { x += x_step; d -= dy2; }
        d += dx2;
    }

    // draw last pixel
    if (m_isSuperSample) {
        DrawPixelSuperSampled(x1, y1, c);
    }
    else {
        DrawPixel(x1, y1, c);
    }
}

void Renderer::DrawPixel(int x, int y, const Color& c) {
    if (x < 0 || y < 0 || x >= m_width || y >= m_height) return;
    m_outBuffer[INDEX(m_width, x, y, 0)] = c[0];
    m_outBuffer[INDEX(m_width, x, y, 1)] = c[1];
    m_outBuffer[INDEX(m_width, x, y, 2)] = c[2];
}

void Renderer::DrawPixelSuperSampled(int x, int y, const Color& c) {
    if (x < 0 || y < 0 || x >= m_width * m_factorSuperSample || y >= m_height * m_factorSuperSample) return;
    m_outBufferSuperSample[INDEX(m_width * m_factorSuperSample, x, y, 0)] = c[0];
    m_outBufferSuperSample[INDEX(m_width * m_factorSuperSample, x, y, 1)] = c[1];
    m_outBufferSuperSample[INDEX(m_width * m_factorSuperSample, x, y, 2)] = c[2];
}

Color Renderer::CalcColor(const Material& material, const vec3& surface_position, const vec3& surface_normal) {
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
            if (dot(l, surface_normal) < 0 && !drawBackshadow) continue;
            const vec3 v = normalize(getPosition(transform_camera_inverse) - surface_position);
            const vec3 r = l - 2 * dot(l, surface_normal) * surface_normal;

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

void Renderer::InitOpenGLRendering() {
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

void Renderer::CreateOpenGLBuffer() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gScreenTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_width, m_height, 0, GL_RGB, GL_FLOAT, NULL);
    glViewport(0, 0, m_width, m_height);
}

void Renderer::SwapBuffers() {
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

void Renderer::ClearColorBuffer() {
    if (m_outBuffer) memset(m_outBuffer, 0, sizeof(float) * 3 * m_width * m_height);
    if (m_outBufferSuperSample) memset(m_outBufferSuperSample, 0, sizeof(float) * m_factorSuperSample * m_factorSuperSample * 3 * m_width * m_height);
}

void Renderer::ClearDepthBuffer() {
    if (m_zbuffer) memset(m_zbuffer, FLT_MAX, sizeof(float) * m_width * m_height);
    if (m_zbufferSuperSample) memset(m_zbufferSuperSample, 0, sizeof(float) * m_factorSuperSample * m_factorSuperSample * m_width * m_height);
}

void Renderer::ClearBloomBuffer() {
    if (m_isBloom) {
        int width = m_width,
            height = m_height;
        if (m_isSuperSample) {
            width *= m_factorSuperSample;
            height *= m_factorSuperSample;
        }
        if (m_bloomBuffer) delete m_bloomBuffer; // has to be reset every time, depending on whether or not we're supersampling
        m_bloomBuffer = new float[3 * width * height];
    }
}

void Renderer::setSupersampling(bool isSupersampling, int factorSuperSample) {
    m_isSuperSample = isSupersampling;
    m_factorSuperSample = factorSuperSample;
    if (m_outBufferSuperSample) delete m_outBufferSuperSample;
    if (m_zbufferSuperSample) delete m_zbufferSuperSample;
    m_outBufferSuperSample = new float[m_factorSuperSample * m_factorSuperSample * 3 * m_width * m_height];
    m_zbufferSuperSample = new float[m_factorSuperSample * m_factorSuperSample * m_width * m_height];
    ClearColorBuffer();
    ClearDepthBuffer();
    ClearBloomBuffer();
}

void Renderer::setBloom(bool isBloom, float threshBloom, int spreadBloom) {
    m_isBloom = isBloom;
    ClearBloomBuffer();
    m_threshBloom = threshBloom;
    m_spreadBloom = spreadBloom;
    if (m_weightsBloom) delete m_weightsBloom;
    m_weightsBloom = new float[spreadBloom * 2 + 1];
    float sum = 0;
    for (int i = 0; i < m_spreadBloom * 2 + 1; i++) {
        float d = i - (m_spreadBloom + 1); // casting to float because we don't want integer division
        m_weightsBloom[i] = exp(d * d / (2 * m_spreadBloom * m_spreadBloom));
        sum += m_weightsBloom[i];
    }
    for (int i = 0; i < m_spreadBloom * 2 + 1; i++) {
        m_weightsBloom[i] /= sum;
    }
}

void Renderer::applyEffects() {
    int width = m_width,
        height = m_height;
    if (m_isSuperSample) {
        width *= m_factorSuperSample;
        height *= m_factorSuperSample;
    }
    if (m_isBloom) {
        // extract bright pixels
        memset(m_bloomBuffer, 0, sizeof(float) * 3 * width * height);
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                for (int c = 0; c < 3; c++) {
                    if (m_isSuperSample) {
                        if (m_outBufferSuperSample[INDEX(width, x, y, c)] > m_threshBloom) {
                            m_bloomBuffer[INDEX(width, x, y, c)] = m_outBufferSuperSample[INDEX(width, x, y, c)];
                        }
                    }
                    else if (m_outBuffer[INDEX(width, x, y, c)] > m_threshBloom) {
                        m_bloomBuffer[INDEX(width, x, y, c)] = m_outBuffer[INDEX(width, x, y, c)];
                    }
                }
            }
        }
        // horizontal pass
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                for (int c = 0; c < 3; c++) {
                    for (int i = 0; i < m_spreadBloom * 2 + 1; i++) {
                        int x_curr = x + i - m_spreadBloom;
                        if (x_curr > 0 && x_curr < width) {
                            m_bloomBuffer[INDEX(width, x_curr, y, c)] += m_bloomBuffer[INDEX(width, x, y, c)] * m_weightsBloom[i];
                        }
                    }
                }
            }
        }
        // vertical pass
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                for (int c = 0; c < 3; c++) {
                    for (int i = 0; i < m_spreadBloom * 2 + 1; i++) {
                        int y_curr = y + i - m_spreadBloom;
                        if (y_curr > 0 && y_curr < height) {
                            m_bloomBuffer[INDEX(width, x, y_curr, c)] += m_bloomBuffer[INDEX(width, x, y, c)] * m_weightsBloom[i];
                        }
                    }
                }
            }
        }
        blendBloomBuffer();
    }

    if (m_isSuperSample) {
        const float factorPixel = 1.f / (m_factorSuperSample * m_factorSuperSample);
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                int i = x / m_factorSuperSample;
                int j = y / m_factorSuperSample;
                for (int c = 0; c < 3; c++) {
                    m_outBuffer[INDEX(m_width, i, j, c)] += m_outBufferSuperSample[INDEX(width, x, y, c)] * factorPixel;
                }
            }
        }
    }
}

void Renderer::blendBloomBuffer() {
    int width = m_width,
        height = m_height;
    if (m_isSuperSample) {
        width *= m_factorSuperSample;
        height *= m_factorSuperSample;
    }
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            for (int c = 0; c < 3; c++) {
                if (m_isSuperSample) {
                    const float v = m_outBufferSuperSample[INDEX(width, x, y, c)] + m_bloomBuffer[INDEX(width, x, y, c)];
                    m_outBufferSuperSample[INDEX(width, x, y, c)] = 1.f - 1 / (1 + v); // fit between 0 and 1
                }
                else {
                    const float v = m_outBuffer[INDEX(width, x, y, c)] + m_bloomBuffer[INDEX(width, x, y, c)];
                    m_outBuffer[INDEX(width, x, y, c)] = 1.f - 1 / (1 + v); // fit between 0 and 1
                }
            }
        }
    }
}

void Renderer::setFog(bool isFog, Color fogColor, float fogMinDistance, float fogMaxDistance) {
    m_isFog = isFog;
    m_fogColor = fogColor;
    m_fogMinDistance = fogMinDistance;
    m_fogMaxDistance = fogMaxDistance;
}
