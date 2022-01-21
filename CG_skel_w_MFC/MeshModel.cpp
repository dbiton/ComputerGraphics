#include "stdafx.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include "mat.h"
#include "MeshModel.h"
#include "PrimMeshModel.h"
#include "InitShader.h"
#include "vec.h"

using namespace std;

Face faceFromStream(std::istream& aStream)
{
	Face face;

	char c;
	for (int i = 0; i < 3; i++)
	{
		aStream >> std::ws >> face.v[i] >> std::ws;
		if (aStream.peek() != '/')
			continue;
		aStream >> c >> std::ws;
		if (aStream.peek() == '/')
		{
			aStream >> c >> std::ws >> face.vn[i];
			continue;
		}
		else
			aStream >> face.vt[i];
		if (aStream.peek() != '/')
			continue;
		aStream >> c >> face.vn[i];
	}
	return face;
}

vec3 vec3fFromStream(std::istream & aStream)
{
	float x, y, z;
	aStream >> x >> std::ws >> y >> std::ws >> z;
	return vec3(x, y, z);
}

vec2 vec2fFromStream(std::istream & aStream)
{
	float x, y;
	aStream >> x >> std::ws >> y;
	return vec2(x, y);
}

MeshModel::MeshModel() noexcept { }

MeshModel::MeshModel(string fileName, string modelName)
{
	name = modelName;
	loadFile(fileName);
	fitBoundingBox();
	SetupGL();
}

MeshModel::~MeshModel() { }

void MeshModel::loadFile(string fileName)
{
	std::vector<vec3> positions;
	std::vector<vec3> normals;
	std::vector<vec2> texs;
	std::vector<Face> faces;

	ifstream ifile(fileName.c_str());
	// while not end of file
	while (!ifile.eof())
	{
		// get line
		string curLine;
		getline(ifile, curLine);

		// read type of the line
		istringstream issLine(curLine);
		string lineType;

		issLine >> std::ws >> lineType;

		// based on the type parse data
		if (lineType == "v")
			positions.push_back(vec3fFromStream(issLine));
		else if (lineType == "vn")
			normals.push_back(vec3fFromStream(issLine));
		else if (lineType == "f")
			faces.push_back(faceFromStream(issLine));
		else if (lineType == "vt") {
			texs.push_back(vec2fFromStream(issLine));
		}
		else if (lineType == "#" || lineType == "") {
			// comment / empty line
		}
		else {
			cout << "Found unknown line type \"" << lineType << "\"" << endl;
		}
	}

	processRawVerts(positions, normals, texs, faces);
}

void MeshModel::fitBoundingBox()
{
	vec3 vert_min(FLT_MAX, FLT_MAX, FLT_MAX);
	vec3 vert_max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	if (vertices.size() > 0) {
		for (const auto& vert : vertices) {
			for (int i = 0; i < 3; i++) {
				if (vert_min[i] > vert.position[i]) vert_min[i] = vert.position[i];
				if (vert_max[i] < vert.position[i]) vert_max[i] = vert.position[i];
			}
		}
		bounding_box_min = vert_min;
		bounding_box_max = vert_max;
		Recenter();
	}
}

void MeshModel::Draw()
{
	GLuint loc = glGetUniformLocation(GetProgram(), "inColor");
	glUniform4f(loc, 1, 1, 1, 1);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	if (draw_normals_per_vert) {
		loc = glGetUniformLocation(GetProgram(), "inColor");
		glUniform4f(loc, 0, 1, 0, 1);
		glBindVertexArray(vao_vNormals);
		// calling glDrawArrays instead of glDrawElements because we don't use element array buffers for these
		// admittedly, vertex normals might benefit from an element array buffer, but surface normals really can't, so whatever
		glDrawArrays(GL_LINES, 0, vertices_vNormals.size());
	}
	if (draw_normals_per_face) {
		loc = glGetUniformLocation(GetProgram(), "inColor");
		glUniform4f(loc, 0, 0.5, 1, 1);
		glBindVertexArray(vao_sNormals);
		glDrawArrays(GL_LINES, 0, vertices_sNormals.size());
	}
}

void MeshModel::Recenter() {
	setPosition(self, -(bounding_box_min + bounding_box_max) / 2); // center it in its own axis system!
}

void bindShaderFields() {
	GLuint loc = glGetAttribLocation(GetProgram(), "vPosition");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	loc = glGetAttribLocation(GetProgram(), "vNormal");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	loc = glGetAttribLocation(GetProgram(), "vTex");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, tex));
}

void MeshModel::SetupGL()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
	bindShaderFields();

	glGenVertexArrays(1, &vao_vNormals);
	glGenBuffers(1, &vbo_vNormals);
	glBindVertexArray(vao_vNormals);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vNormals);
	glBufferData(GL_ARRAY_BUFFER, vertices_vNormals.size() * sizeof(Vertex), &vertices_vNormals[0], GL_STATIC_DRAW);
	bindShaderFields();

	glGenVertexArrays(1, &vao_sNormals);
	glGenBuffers(1, &vbo_sNormals);
	glBindVertexArray(vao_sNormals);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_sNormals);
	glBufferData(GL_ARRAY_BUFFER, vertices_sNormals.size() * sizeof(Vertex), &vertices_sNormals[0], GL_STATIC_DRAW);
	bindShaderFields();
}

GLfloat averageLength(vec3 p1, vec3 p2, vec3 p3) noexcept {
	return (length(p1 - p2) + length(p1 - p3) + length(p2 - p3)) / 3;
}

void MeshModel::processRawVerts(const std::vector<vec3>& positions, const std::vector<vec3>& normals, const std::vector<vec2>& texs, const std::vector<Face>& faces)
{
	vertices.clear();
	Vertex v;
	for (int i = 0; i < positions.size(); i++) {
		v.position = positions[i];
		//v.normal = normals[i];
		//v.tex = vec2(0.0);
		vertices.push_back(v);
	}
	for (const auto& face : faces) {
		for (int i = 0; i < 3; i++) indices.push_back(face.v[i] - 1);
		//for (int i = 0; i < 3; i++) indices.push_back(face.vn[i]);
		//for (int i = 0; i < 2; i++) indices.push_back(face.vt[i]);
	}

	// fill in vbo's for drawing normals
	constexpr GLfloat fn_len = 0.5,
					  vn_len = 0.5;
	for (const auto& face : faces) {
		vec3 verts[3];
		for (int i = 0; i < 3; i++) verts[i] = positions[face.v[i] - 1];
		const GLfloat normalsLength = averageLength(verts[0], verts[1], verts[2]);
		for (int i = 0; i < 3; i++) {
			v.position = verts[i];
			vertices_vNormals.push_back(v);
			v.position += normalize(normals[face.vn[i] - 1]) * normalsLength * vn_len;
			vertices_vNormals.push_back(v);
		}
		const vec3 fn = normalize(cross(verts[1] - verts[0], verts[2] - verts[1])) * normalsLength * fn_len, // face normal
			       fm = (verts[0] + verts[1] + verts[2]) / 3; // face mid
		v.position = fm;
		vertices_sNormals.push_back(v);
		v.position += fn;
		vertices_sNormals.push_back(v);
	}
}


Face::Face() : 
	v {0},
	vn{0},
	vt{0}
{
}

Face::Face(int v0, int v1, int v2) : 
	v { v0, v1, v2 },
	vn{ v0, v1, v2 },
	vt{ v0, v1, v2 }
{
}
