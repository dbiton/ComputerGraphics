#include "stdafx.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <map>
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

vec3 vec3fFromStream(std::istream& aStream)
{
    float x, y, z;
    aStream >> x >> std::ws >> y >> std::ws >> z;
    return vec3(x, y, z);
}

vec2 vec2fFromStream(std::istream& aStream)
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

void MeshModel::Draw()
{
    const GLuint inColor = glGetUniformLocation(GetProgram(), "inColor");
    if (IsWireframeMode()) glUniform4f(inColor, 1, 1, 1, 1);
    else glUniform4f(inColor, 0, 0, 0, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vao_size);
    if (draw_normals_per_vert) {
        glUniform4f(inColor, 0, 1, 0, 1);
        glBindVertexArray(vao_vNormals);
        // TODO if we have time, optimize vertices_vNormals and vertices_boundingBox using an EBO, and add lazy init!
        // calling glDrawArrays instead of glDrawElements because we don't use element array buffers for these.
        // admittedly, vertex normals would benefit from an element array buffer (vao_vNormals would be cut in half), but surface normals really can't, so whatever
        glDrawArrays(GL_LINES, 0, vao_vNormals_size);
    }
    if (draw_normals_per_face) {
        glUniform4f(inColor, 0, 0.5, 1, 1);
        glBindVertexArray(vao_sNormals);
        glDrawArrays(GL_LINES, 0, vao_sNormals_size);
    }
    if (draw_bounding_box) {
        glUniform4f(inColor, 1, 0, 0, 1);
        glBindVertexArray(vao_boundingBox);
        // again, this could benefit from an element array buffer, but whatever
        glDrawArrays(GL_LINES, 0, vao_boundingBox_size);
    }
}

void MeshModel::Recenter() {
    setPosition(self, -(bounding_box_min + bounding_box_max) / 2); // center it in its own axis system!
}

void bindShaderFields() noexcept {
    GLuint loc = glGetAttribLocation(GetProgram(), "vPosition");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
    loc = glGetAttribLocation(GetProgram(), "vNormal");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
    loc = glGetAttribLocation(GetProgram(), "fPosition");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, face_middle));
    loc = glGetAttribLocation(GetProgram(), "fNormal");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, face_normal));
    loc = glGetAttribLocation(GetProgram(), "vTex");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, tex));
}

GLfloat averageLength(vec3 p1, vec3 p2, vec3 p3) noexcept {
    return (length(p1 - p2) + length(p1 - p3) + length(p2 - p3)) / 3;
}

// shamelessly stealing this code from opengl-tutorials.com and modifying it because it works so why not lol
struct TripleIndex {
    int vi, vni, vti;
    bool operator<(const TripleIndex that) const noexcept {
        // neat hack that allows comparing arbitrary structs. we need this because std::map requires the first type to implement comparison for it to work
        return memcmp((void*)this, (void*)&that, sizeof(TripleIndex)) > 0;
    };
};

bool tripletExists(const TripleIndex& index, std::map<TripleIndex, GLuint>& tripletToIndex, GLuint& result) {
    std::map<TripleIndex, GLuint>::iterator it = tripletToIndex.find(index);
    if (it == tripletToIndex.end()) {
        return false;
    }
    else {
        result = it->second;
        return true;
    }
}

void indexVBO(const std::vector<vec3>& in_vertices, const std::vector<vec2>& in_uvs, const std::vector<vec3>& in_normals, const std::vector<Face>& faces,
    std::vector<GLuint>& out_indices, std::vector<Vertex>& out_vertices) {
    std::map<TripleIndex, GLuint> tripletToIndex;

    // For each input vertex
    for (GLuint i = 0; i < faces.size(); i++) {
        for (GLuint j = 0; j < 3; j++) {
            TripleIndex triplet = { faces[i].v[j] - 1, faces[i].vn[j] - 1, faces[i].vt[j] - 1 };
            // .obj can support negative indices, going from the end... just gotta hope that's it for "normalizing" these indices...
            // TODO in case we ever get index out of bounds shenanigans here, we'll know we need to refine this normalizing!
            if (triplet.vi < -1) triplet.vi += in_vertices.size() + 1;
            if (triplet.vni < -1) triplet.vi += in_normals.size() + 1;
            if (triplet.vti < -1) triplet.vi += in_uvs.size() + 1;
            GLuint index;

            if (tripletExists(triplet, tripletToIndex, index)) { // A similar vertex is already in the VBO, use it instead !
                out_indices.push_back(index);
            }
            else { // If not, it needs to be added in the output data.
                Vertex v;
                v.position = in_vertices[triplet.vi];
                v.tex = (triplet.vti == -1) ? vec2(0) : in_uvs[triplet.vti]; // no texture! can happen, and we can't assume it's missing for all...
                v.normal = (triplet.vni == -1) ? vec3(0) : in_normals[triplet.vni]; // no normal! can happen, same as above
                out_vertices.push_back(v);
                index = out_vertices.size() - 1;
                out_indices.push_back(index);
                tripletToIndex[triplet] = index;
            }
        }
    }
}

// Within other applications we would definitely use an EBO to reduce memory usage and increase performance,
// but since we ain't getting into geometry shaders, we have no use for EBO's (mostly) because we need face normals and middles...
void MeshModel::processRawVerts(const std::vector<vec3>& positions, const std::vector<vec3>& normals, const std::vector<vec2>& texs, const std::vector<Face>& faces)
{
    std::vector<Vertex> vertices, vertices_vNormals, vertices_sNormals, vertices_boundingBox;
    
    // fill in vbo's for drawing normals
    Vertex v;
    constexpr GLfloat fn_len = 0.5,
        /*          */vn_len = 0.5;
    for (const auto& face : faces) {
        vec3 verts[3];
        for (int i = 0; i < 3; i++) verts[i] = positions[face.v[i] - 1];
        const GLfloat normalsLength = averageLength(verts[0], verts[1], verts[2]);
        const vec3 fn_real = normalize(cross(verts[1] - verts[0], verts[2] - verts[1])), // face normal
            /*   */fn = fn_real * normalsLength * fn_len, // face normal, resized for visibility
            /*   */fm = (verts[0] + verts[1] + verts[2]) / 3; // face mid
        for (int i = 0; i < 3; i++) {
            v.position = verts[i];
            v.tex = (face.vt[i] == 0) ? vec2(0) : texs[face.vt[i] - 1]; // could get no texture, and we can't assume it's missing for all...
            v.normal = (face.vn[i] == 0) ? vec3(0) : normals[face.vn[i] - 1]; // could get no normal, same as above
            v.face_normal = fn_real;
            v.face_middle = fm;
            vertices.push_back(v);
            vertices_vNormals.push_back(v);
            v.position += (face.vn[i] == 0) ? vec3(0) : normalize(v.normal) * normalsLength * vn_len;
            vertices_vNormals.push_back(v);
        }
        v.position = fm;
        vertices_sNormals.push_back(v);
        v.position += fn;
        vertices_sNormals.push_back(v);
    }

    // create the bounding box vbo, yes i know this is disgusting copypaste code but i cba to make this any prettier
    // also for some reason nothing renders right if we don't run this code, Recenter() is probably crucial here
    vec3 vert_min(FLT_MAX);
    vec3 vert_max(-FLT_MAX);
    for (const auto& vert : vertices) {
        for (int i = 0; i < 3; i++) {
            if (vert_min[i] > vert.position[i]) vert_min[i] = vert.position[i];
            if (vert_max[i] < vert.position[i]) vert_max[i] = vert.position[i];
        }
    }
    bounding_box_min = vert_min;
    bounding_box_max = vert_max;
    Recenter();
    const vec3 v000 = bounding_box_min,
        /*   */v100 = vec3(bounding_box_max.x, bounding_box_min.y, bounding_box_min.z),
        /*   */v010 = vec3(bounding_box_min.x, bounding_box_max.y, bounding_box_min.z),
        /*   */v110 = vec3(bounding_box_max.x, bounding_box_max.y, bounding_box_min.z),
        /*   */v001 = vec3(bounding_box_min.x, bounding_box_min.y, bounding_box_max.z),
        /*   */v101 = vec3(bounding_box_max.x, bounding_box_min.y, bounding_box_max.z),
        /*   */v011 = vec3(bounding_box_min.x, bounding_box_max.y, bounding_box_max.z),
        /*   */v111 = bounding_box_max;

    v.position = v000;
    vertices_boundingBox.push_back(v);
    v.position = v100;
    vertices_boundingBox.push_back(v);
    v.position = v000;
    vertices_boundingBox.push_back(v);
    v.position = v010;
    vertices_boundingBox.push_back(v);
    v.position = v000;
    vertices_boundingBox.push_back(v);
    v.position = v001;
    vertices_boundingBox.push_back(v);

    v.position = v110;
    vertices_boundingBox.push_back(v);
    v.position = v100;
    vertices_boundingBox.push_back(v);
    v.position = v101;
    vertices_boundingBox.push_back(v);
    v.position = v100;
    vertices_boundingBox.push_back(v);
    v.position = v110;
    vertices_boundingBox.push_back(v);
    v.position = v010;
    vertices_boundingBox.push_back(v);
    v.position = v011;
    vertices_boundingBox.push_back(v);
    v.position = v010;
    vertices_boundingBox.push_back(v);
    v.position = v101;
    vertices_boundingBox.push_back(v);
    v.position = v001;
    vertices_boundingBox.push_back(v);
    v.position = v011;
    vertices_boundingBox.push_back(v);
    v.position = v001;
    vertices_boundingBox.push_back(v);

    v.position = v111;
    vertices_boundingBox.push_back(v);
    v.position = v011;
    vertices_boundingBox.push_back(v);
    v.position = v111;
    vertices_boundingBox.push_back(v);
    v.position = v101;
    vertices_boundingBox.push_back(v);
    v.position = v111;
    vertices_boundingBox.push_back(v);
    v.position = v110;
    vertices_boundingBox.push_back(v);

    vao_size = vertices.size();
    vao_vNormals_size = vertices_vNormals.size();
    vao_sNormals_size = vertices_sNormals.size();
    vao_boundingBox_size = vertices_boundingBox.size();

    GLuint vbo, vbo_vNormals, vbo_sNormals, vbo_boundingBox;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vao_size * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    bindShaderFields();

    glGenVertexArrays(1, &vao_vNormals);
    glGenBuffers(1, &vbo_vNormals);
    glBindVertexArray(vao_vNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vNormals);
    glBufferData(GL_ARRAY_BUFFER, vao_vNormals_size * sizeof(Vertex), &vertices_vNormals[0], GL_STATIC_DRAW);
    bindShaderFields();

    glGenVertexArrays(1, &vao_sNormals);
    glGenBuffers(1, &vbo_sNormals);
    glBindVertexArray(vao_sNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_sNormals);
    glBufferData(GL_ARRAY_BUFFER, vao_sNormals_size * sizeof(Vertex), &vertices_sNormals[0], GL_STATIC_DRAW);
    bindShaderFields();

    glGenVertexArrays(1, &vao_boundingBox);
    glGenBuffers(1, &vbo_boundingBox);
    glBindVertexArray(vao_boundingBox);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_boundingBox);
    glBufferData(GL_ARRAY_BUFFER, vao_boundingBox_size * sizeof(Vertex), &vertices_boundingBox[0], GL_STATIC_DRAW);
    bindShaderFields();
}

Face::Face() : v{ 0 }, vn{ 0 }, vt{ 0 } { }

Face::Face(int v0, int v1, int v2) : v{ v0, v1, v2 }, vn{ 0 }, vt{ 0 } { }
