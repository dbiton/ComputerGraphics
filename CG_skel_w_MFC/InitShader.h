#pragma once

struct Material;
struct Light;

static char* readShaderSource(const char* shaderFile);
void InitShader(const char* vShaderFile, const char* fShaderFile);

GLuint GetProgram() noexcept;
void ToggleWireframes() noexcept;
bool IsWireframeMode() noexcept;

void shaderSetFloat(std::string name, GLfloat v);
void shaderSetMat4(std::string name, GLfloat *v);
void shaderSetVec3(std::string name, GLfloat* v);
void shaderSetInt(std::string name, int v);
void shaderSetMaterial(Material* material);
void shaderSetLight(Light* light, int index);