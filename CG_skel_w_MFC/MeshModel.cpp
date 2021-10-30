#include "StdAfx.h"
#include "MeshModel.h"
#include "vec.h"
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

MeshModel::MeshModel() : 
	is_draw_normals_per_vert(false),
	is_draw_normals_per_face(false),
	is_draw_bounding_box(false)
{
}

MeshModel::MeshModel(string fileName)
{
	loadFile(fileName);
	fitBoundingBox();
	calculateFaceNormals();
}

MeshModel::~MeshModel(void)
{
}

void MeshModel::loadFile(string fileName)
{
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
}

void MeshModel::setDrawNormalsPerVert(bool b)
{
	is_draw_normals_per_vert = b;
}

void MeshModel::setDrawNormalsPerFace(bool b)
{
	is_draw_normals_per_face = b;
}

void MeshModel::setDrawBoundingBox(bool b)
{
	is_draw_bounding_box = b;
}


void MeshModel::draw()
{
	
}

void MeshModel::fitBoundingBox()
{
	if (verts.size() > 0) {
		vec3 vert_min(FLT_MAX, FLT_MAX, FLT_MAX);
		vec3 vert_max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		for (const auto& vert : verts) {
			for (int i = 0; i < 3; i++) {
				if (vert_min[i] > vert[i]) vert_min[i] = vert[i];
				if (vert_max[i] < vert[i]) vert_max[i] = vert[i];
			}
		}
		bounding_box.box_max = vert_max;
		bounding_box.box_min = vert_min;
	}
	else {
		bounding_box.box_max = vec3(0, 0, 0);
		bounding_box.box_min = vec3(0, 0, 0);
	}
}

void MeshModel::calculateFaceNormals()
{
	for (auto& face : faces) {
		// load face verts
		vec3 p[3];
		for (int i = 0; i < 3; i++) {
			p[i] = verts[face.v[i]-1];
		}
		face.face_mid = (p[0] + p[1] + p[2]) / 3.f;
		face.face_normal = normalize(cross((p[1] - p[0]), p[2] - p[0]));
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
