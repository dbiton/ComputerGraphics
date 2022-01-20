#pragma once
GLuint GetProgram();
static char* readShaderSource(const char* shaderFile);
void InitShader(const char* vShaderFile, const char* fShaderFile);