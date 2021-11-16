#include "StdAfx.h"
#include "MeshModel.h"
#include "PrimMeshModel.h"
#include "vec.h"
#include "mat.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

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

MeshModel::MeshModel() noexcept :
	is_draw_normals_per_vert(false),
	is_draw_normals_per_face(false),
	is_draw_bounding_box(false),
	color_mesh(1.0, 1.0, 1.0),
	color_vert_normal(1.0, 0.0, 0.0),
	color_face_normal(0.0, 1.0, 0.0),
	color_bounding_box(0.0, 0.0, 1.0)
{
}

MeshModel::MeshModel(string fileName)
{
	loadFile(fileName);
	fitBoundingBox();
}

MeshModel::~MeshModel(void)
{
}

void MeshModel::loadFile(string fileName)
{
	std::vector<vec3> verts;
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
		if (lineType == "v" || lineType == "vn" || lineType == "vt")
			verts.push_back(vec3fFromStream(issLine));
		else if (lineType == "f")
			faces.push_back(faceFromStream(issLine));
		else if (lineType == "#" || lineType == "")
		{
			// comment / empty line
		}
		else
		{
			cout << "Found unknown line type \"" << lineType << "\"" << endl;
		}
	}

	processRawVerts(verts, faces);
}

void MeshModel::SetDrawNormalsPerVert(bool b)
{
	is_draw_normals_per_vert = b;
}

void MeshModel::SetDrawNormalsPerFace(bool b)
{
	is_draw_normals_per_face = b;
}

void MeshModel::SetDrawBoundingBox(bool b)
{
	is_draw_bounding_box = b;
}

void MeshModel::ToggleDrawNormalsPerVert()
{
	is_draw_normals_per_vert = !is_draw_normals_per_vert;
}

void MeshModel::ToggleDrawNormalsPerFace()
{
	is_draw_normals_per_face = !is_draw_normals_per_face;
}

void MeshModel::ToggleDrawBoundingBox()
{
	is_draw_bounding_box = !is_draw_bounding_box;
}

void MeshModel::draw(Renderer* renderer, bool isActiveModel)
{
	renderer->SetObjectTransform(getTransform());
	renderer->DrawTriangles(vertices, isActiveModel, is_draw_normals_per_face, is_draw_normals_per_vert);
	if (is_draw_bounding_box) renderer->DrawBox(bounding_box_min, bounding_box_max);
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
		setPosition((bounding_box_min + bounding_box_max) / 2);
	}
}

void MeshModel::processRawVerts(const std::vector<vec3>& verts, const std::vector<Face>& faces)
{
	vertices.clear();
	for (const auto& face : faces) {
		Vertex v0, v1, v2;

		v0.position = verts[face.v[0] - 1];
		v1.position = verts[face.v[1] - 1];
		v2.position = verts[face.v[2] - 1];

		v0.normal = verts[face.vn[0] - 1];
		v1.normal = verts[face.vn[1] - 1];
		v2.normal = verts[face.vn[2] - 1];

		vertices.push_back(v0);
		vertices.push_back(v1);
		vertices.push_back(v2);
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