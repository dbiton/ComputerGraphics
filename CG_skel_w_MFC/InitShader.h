#pragma once
static char* readShaderSource(const char* shaderFile);
void InitShader(const char* vShaderFile, const char* fShaderFile);

GLuint GetProgram() noexcept;
void ToggleWireframes() noexcept;
bool IsWireframeMode() noexcept;
