#include "stdafx.h"
#include "PrimMeshModel.h"

PrimMeshModel::PrimMeshModel() :
    MeshModel()
{
}

PrimMeshModel PrimMeshModel::Cube(vec3 p, GLfloat len)
{
    return Cuboid(p, vec3(len, len, len));
}

PrimMeshModel PrimMeshModel::Cuboid(vec3 p, vec3 dim)
{
    std::vector<vec3> verts; 
    std::vector<Face> faces;
    PrimMeshModel cuboid;
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
                const vec3 offset(dim.x * 0.5 * x, dim.y * 0.5 * y, dim.z * 0.5 * z);
                const vec3 vert = p + offset;
                verts.push_back(vert);
            }
        }
    }
    // quad 0
    faces.push_back(Face(1, 2, 5));
    faces.push_back(Face(5, 2, 6));
    // quad 1
    faces.push_back(Face(2, 4, 6));
    faces.push_back(Face(6, 4, 8));
    // quad 2
    faces.push_back(Face(3, 4, 7));
    faces.push_back(Face(7, 4, 8));
    // quad 4
    faces.push_back(Face(1, 3, 5));
    faces.push_back(Face(5, 3, 7));
    // quad 5
    faces.push_back(Face(5, 6, 7));
    faces.push_back(Face(7, 6, 8));
    // quad 6
    faces.push_back(Face(1, 2, 3));
    faces.push_back(Face(3, 2, 4));

    cuboid.processRawVerts(verts, faces);
    cuboid.fitBoundingBox();

    return cuboid;
}

PrimMeshModel PrimMeshModel::Pyramid(vec3 p, GLfloat height, GLfloat base_radius, int base_sides)
{
    std::vector<vec3> verts;
    std::vector<Face> faces;
    PrimMeshModel pyramid;
    /*
        verts:
        0: base 0
        1: base 1
        2: base 2
        3: ...
        base_sides: top
    */
    
    for (int i = 0; i < base_sides; i++) {
        const GLfloat angle = 2 * M_PI * i / base_sides;
        const vec3 v(cos(angle) * base_radius, sin(angle) * base_radius, -height / 2);
        verts.push_back(v);
        if (i < base_sides - 1) {
            // side face
            faces.push_back(Face(i + 1, i + 2, base_sides + 1));
            // base face
            if (i > 0) {
                faces.push_back(Face(i + 1, i + 2, 1));
            }
        }
    }
    // one last face and top vertex
    faces.push_back(Face(base_sides, 1, base_sides + 1));
    const vec3 top = p + vec3(0, 0, height / 2);
    verts.push_back(top);

    pyramid.processRawVerts(verts, faces);
    pyramid.fitBoundingBox();

    return pyramid;
}

PrimMeshModel PrimMeshModel::Prism(vec3 p, GLfloat height, GLfloat base_radius, int base_sides)
{
    std::vector<vec3> verts;
    std::vector<Face> faces;
    PrimMeshModel prism;
    /*
        verts:
        0: base 0 vert 0
        1: base 0 vert 1
        2: base 0 vert 2
        3: ...
        base_sides: base 1 vert 0
        base_sides + 1: base 1 vert 1
        base_sides + 2: base 1 vert 2
        ...
    */
    for (int side = 0; side < 2; side++) {
        for (int i = 0; i < base_sides; i++) {
            // create vertice
            const GLfloat angle = 2 * M_PI * i / base_sides;
            const vec3 v(cos(angle) * base_radius, sin(angle) * base_radius, (side - 0.5) * height);
            verts.push_back(v);
            // create face
            if (i < base_sides - 1) {
                // side face
                faces.push_back(Face(i + 1 + base_sides * side, i + 2 + base_sides * side, i + 1 + (1 - side) * (base_sides + 1)));
                // base face
                if (i > 0) {
                    faces.push_back(Face(i + 1 + base_sides * side, i + 2 + base_sides * side, 1 + base_sides * side));
                }
            }
        }
    }
    // two last faces
    faces.push_back(Face(base_sides, 1, base_sides + 1));
    faces.push_back(Face(1, base_sides + 1, base_sides * 2 - 1));


    prism.processRawVerts(verts, faces);
    prism.fitBoundingBox();

    return prism;
}

PrimMeshModel PrimMeshModel::Sphere(vec3 p, GLfloat radius, int subdivisions)
{
    for (int i = 0; i < subdivisions; i++) {
        for (int j = 0; j < subdivisions; j++) {

        }
    }
    return Cube(p, radius);
}

// cubes and pyramids (and tetrahedrons, icosahedrons, etc if we want) should be simple enough to define, but spheres
// (and maybe cylinders, paraboloids etc if we want) require us to settle on what LOD (Level Of Detail) to use when approximating
// their perfect curvature... maybe implement the same shape but with multiple different LOD's? could be interesting
// of course we only need at least one, and we should probably start with the cube
