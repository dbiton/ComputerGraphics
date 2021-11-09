#include "StdAfx.h"
#include "MeshModel.h"
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

MeshModel::MeshModel() : 
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
	calculateFaceNormals();
	calculateTriangles();
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
	calculateTriangles();
}

void MeshModel::setDrawNormalsPerFace(bool b)
{
	is_draw_normals_per_face = b;
	calculateTriangles();
}

void MeshModel::setDrawBoundingBox(bool b)
{
	is_draw_bounding_box = b;
	calculateTriangles();
}

void MeshModel::draw(Renderer* renderer)
{
	// normals, colors etc need to be taken care of
	renderer->DrawTriangles(&renderVertices.triangles);
}

void MeshModel::getFaceVerts(const Face& face, vec3& v0, vec3& v1, vec3& v2)
{
	v0 = verts[face.v[0] - 1];
	v1 = verts[face.v[1] - 1];
	v2 = verts[face.v[2] - 1];
}

inline void MeshModel::getFaceVertNormals(const Face& face, vec3& v0, vec3& v1, vec3& v2)
{
	v0 = verts[face.vn[0] - 1];
	v1 = verts[face.vn[1] - 1];
	v2 = verts[face.vn[2] - 1];
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
		vec3 p0, p1, p2;
		getFaceVerts(face, p0, p1, p2);
		face.face_mid = (p0 + p1 + p2) / 3.f;
		face.face_normal = normalize(cross((p1 - p0), p2 - p0));
	}
}

void MeshModel::calculateTriangles()
{
	renderVertices.clear();

	for (auto const& face : faces) {
		vec3 v3s[3];
		getFaceVerts(face, v3s[0], v3s[1], v3s[2]);
		for (int i = 0; i < 3; i++) {
			v3s[i] = applyTransformToPoint(world_transform, v3s[i]);
			renderVertices.triangles.push_back(v3s[i]);
			renderVertices.triangles_colors.push_back(color_mesh);
		}
	}
	/*
	if (is_draw_normals_per_vert) {
		for (auto const& face : faces) {
			vec3 v3s[3];
			getFaceVertNormals(face, v3s[0], v3s[1], v3s[2]);
			for (int i = 0; i < 3; i++) {
				vec4 v4(v3s[i].x, v3s[i].y, v3s[i].z, 1);
				v4 = mvmult(world_transform, v4);
				renderVertices.triangles.push_back(v4);
				triangles.colors.push_back(color_vert_normal);
			}
		}
	}

	if (is_draw_normals_per_face) {
		for (auto const& face : faces) {
			vec4 v4s[3];
			triangleFromNormal(face.face_mid, face.face_normal, 1, 4, v4s[0], v4s[1], v4s[2]);
			for (int i = 0; i < 3; i++) {
				v4s[i] = mvmult(world_transform, v4s[i]);
				triangles.vertices.push_back(v4s[i]);
				triangles.colors.push_back(color_face_normal);
			}
		}
	}
	*/
	if (is_draw_bounding_box) {
		const vec3 min = bounding_box.box_min;
		const vec3 max = bounding_box.box_max;
		renderVertices.triangles.push_back(applyTransformToPoint(world_transform, vec3(min.x, min.y, min.z)));
		renderVertices.triangles.push_back(applyTransformToPoint(world_transform, vec3(max.x, min.y, min.z)));
		renderVertices.triangles.push_back(applyTransformToPoint(world_transform, vec3(min.x, max.y, min.z)));
		renderVertices.triangles.push_back(applyTransformToPoint(world_transform, vec3(max.x, max.y, min.z)));
		renderVertices.triangles.push_back(applyTransformToPoint(world_transform, vec3(min.x, min.y, max.z)));
		renderVertices.triangles.push_back(applyTransformToPoint(world_transform, vec3(max.x, min.y, max.z)));
		renderVertices.triangles.push_back(applyTransformToPoint(world_transform, vec3(min.x, max.y, max.z)));
		renderVertices.triangles.push_back(applyTransformToPoint(world_transform, vec3(max.x, max.y, max.z)));
		for (int i = 0; i < 8; i++) renderVertices.triangles_colors.push_back(color_bounding_box);
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

void RenderVertices::pushTriangle(vec3 v0, vec3 v1, vec3 v2, vec3 c0, vec3 c1, vec3 c2)
{
}

void RenderVertices::pushNormal(vec3 src, vec3 dst, vec3 c_src, vec3 c_dst)
{
}

void RenderVertices::clear()
{
}
