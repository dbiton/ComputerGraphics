#include "stdafx.h"

#include <iostream>
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/freeglut_ext.h"
#include "InitShader.h"

#include "Material.h"
#include "Scene.h"

static GLuint program;
static bool isWireframeMode; 

// Create a NULL-terminated string by reading the provided file
static char*
readShaderSource(const char* shaderFile)
{
    FILE* fp = fopen(shaderFile, "r");

    if ( fp == NULL ) { return NULL; }

    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);

    fseek(fp, 0L, SEEK_SET);
    char* buf = new char[size + 1];
    fread(buf, 1, size, fp);

    buf[size] = '\0';
    fclose(fp);

    return buf;
}


// Create a GLSL program object from vertex and fragment shader files
void
InitShader(const char* vShaderFile, const char* fShaderFile)
{
    struct Shader {
	const char*  filename;
	GLenum       type;
	GLchar*      source;
    }  shaders[2] = {
	{ vShaderFile, GL_VERTEX_SHADER, NULL },
	{ fShaderFile, GL_FRAGMENT_SHADER, NULL }
    };

    program = glCreateProgram();
    
    for ( int i = 0; i < 2; ++i ) {
	Shader& s = shaders[i];
	s.source = readShaderSource( s.filename );
	if ( shaders[i].source == NULL ) {
	    std::cerr << "Failed to read " << s.filename << std::endl;
	    exit( EXIT_FAILURE );
	}
	
	GLuint shader = glCreateShader( s.type );
	glShaderSource( shader, 1, (const GLchar**) &s.source, NULL );
	glCompileShader( shader );

	GLint  compiled;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
	if ( !compiled ) {
	    std::cerr << s.filename << " failed to compile:" << std::endl;
	    GLint  logSize;
	    glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize );
	    char* logMsg = new char[logSize];
	    glGetShaderInfoLog( shader, logSize, NULL, logMsg );
	    std::cerr << logMsg << std::endl;
	    delete [] logMsg;

	    exit( EXIT_FAILURE );
	}

	delete [] s.source;

	glAttachShader( program, shader );
    }

    /* link  and error check */
    glLinkProgram(program);

    GLint  linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    if ( !linked ) {
	std::cerr << "Shader program failed to link" << std::endl;
	GLint  logSize;
	glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logSize);
	char* logMsg = new char[logSize];
	glGetProgramInfoLog( program, logSize, NULL, logMsg );
	std::cerr << logMsg << std::endl;
	delete [] logMsg;

	exit( EXIT_FAILURE );
    }

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glPolygonMode(GL_FRONT_AND_BACK, (isWireframeMode = false) ? GL_LINE : GL_FILL);

    /* use program object */
    glUseProgram(program);
}

GLuint GetProgram() noexcept { return program; }

void ToggleWireframes() noexcept {
	isWireframeMode = !isWireframeMode;
	glPolygonMode(GL_FRONT_AND_BACK, isWireframeMode ? GL_LINE : GL_FILL);
}

bool IsWireframeMode() noexcept { return isWireframeMode; }

void shaderSetFloat(std::string name, GLfloat v)
{
	GLuint loc = glGetUniformLocation(GetProgram(), name.c_str());
	glUniform1f(loc, v);
}

void shaderSetMat4(std::string name, GLfloat* v) {
	GLuint loc = glGetUniformLocation(GetProgram(), name.c_str());
	glUniformMatrix4fv(loc, 1, GL_TRUE, v);
}

void shaderSetVec3(std::string name, GLfloat* v) {
	GLuint loc = glGetUniformLocation(GetProgram(), name.c_str());
	glUniform3fv(loc, 1, v);
}

void shaderSetInt(std::string name, int v)
{
	GLuint loc = glGetUniformLocation(GetProgram(), name.c_str());
	glUniform1i(loc, v);
}

void shaderSetMaterial(Material* material) {
	shaderSetVec3("material.diffuse", material->diffuse);
	shaderSetVec3("material.specular", material->specular);
	shaderSetVec3("material.ambient", material->emissive);
	shaderSetFloat("material.shininess", material->shininess);
	shaderSetFloat("material.roughness", material->roughness);
	shaderSetFloat("material.ambient_reflect", material->ambient_reflect);
}

void shaderSetLight(Light* light, int index)
{
	std::string l = "lights[" + std::to_string(index) + "].";
	shaderSetVec3(l + "diffuse", light->diffuse);
	shaderSetVec3(l + "specular", light->specular);
	shaderSetVec3(l + "ambient", light->ambient);
	shaderSetVec3(l + "position", light->position);
	shaderSetFloat(l + "brightness", light->brightness);
	shaderSetInt(l+"isDirectional", light->isDirectional);
}