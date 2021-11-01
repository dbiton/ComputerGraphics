#include "stdafx.h"
#include "PrimMeshModel.h"

PrimMeshModel::PrimMeshModel() :
    MeshModel()
{
}

PrimMeshModel PrimMeshModel::Cube(vec3 p, GLfloat len)
{
    return Box(p, vec3(len, len, len));
}

PrimMeshModel PrimMeshModel::Box(vec3 p, vec3 dim)
{
    PrimMeshModel box;
    /*
        verts:
        0: (-1,-1,-1)
        1: (-1,-1, 1)
        2: (-1, 1,-1)
        3: (-1, 1, 1)
        4: ( 1,-1,-1)
        5: ( 1,-1, 1)
        6: ( 1, 1,-1)
        7: ( 1, 1, 1)
    */
    for (int x = -1; x <= 1; x += 2) {
        for (int y = -1; y <= 1; y += 2) {
            for (int z = -1; z <= 1; z += 2) {
                const vec3 offset(dim.x * 0.5 * x, dim.y * 0.5 * y, dim.z * 0.5 * y);
                const vec3 vert = p + offset;
                box.verts.push_back(vert);
            }
        }
    }
    // quad 0
    box.faces.push_back(Face(1, 2, 5));
    box.faces.push_back(Face(5, 2, 6));
    // quad 1
    box.faces.push_back(Face(2, 4, 6));
    box.faces.push_back(Face(6, 4, 8));
    // quad 2
    box.faces.push_back(Face(3, 4, 7));
    box.faces.push_back(Face(7, 4, 8));
    // quad 4
    box.faces.push_back(Face(1, 3, 5));
    box.faces.push_back(Face(5, 3, 7));
    // quad 5
    box.faces.push_back(Face(5, 6, 7));
    box.faces.push_back(Face(7, 6, 8));
    // quad 6
    box.faces.push_back(Face(1, 2, 3));
    box.faces.push_back(Face(3, 2, 4));

    box.fitBoundingBox();
    box.calculateFaceNormals();
    box.calculateTriangles();

    return box;
}
