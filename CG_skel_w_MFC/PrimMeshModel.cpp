#include "stdafx.h"

#include "PrimMeshModel.h"

PrimMeshModel::PrimMeshModel() : MeshModel() { }

PrimMeshModel PrimMeshModel::Cube(vec3 p, GLfloat len)
{
    return Cuboid(p, vec3(len, len, len));
}

PrimMeshModel PrimMeshModel::Cuboid(vec3 p, vec3 dim)
{
    std::vector<vec3> verts;
    std::vector<Face> faces;
    PrimMeshModel cuboid;
    cuboid.name = "Cuboid";
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

    cuboid.processRawVerts(verts, verts, faces);
    cuboid.fitBoundingBox();

    return cuboid;
}

PrimMeshModel PrimMeshModel::Pyramid(vec3 p, GLfloat height, GLfloat base_radius, int base_sides)
{
    std::vector<vec3> verts;
    std::vector<Face> faces;
    PrimMeshModel pyramid;
    pyramid.name = "Pyramid";
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

    pyramid.processRawVerts(verts, verts, faces);
    pyramid.fitBoundingBox();

    return pyramid;
}

PrimMeshModel PrimMeshModel::Prism(vec3 p, GLfloat height, GLfloat base_radius, int base_sides)
{
    std::vector<vec3> verts;
    std::vector<Face> faces;
    PrimMeshModel prism;
    prism.name = "Prism";
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
        }
    }

    faces.push_back(Face(1, 2, 1 + base_sides + 1));
    faces.push_back(Face(1 + base_sides, 2 + base_sides, 1));
    for (int i = 1; i < base_sides - 1; i++) {
        // side face
        faces.push_back(Face(i + 1 + base_sides, i + 2 + base_sides, i + 1));
        faces.push_back(Face(i + 1, i + 2, i + 1 + base_sides + 1));
        // base face
        faces.push_back(Face(1 + base_sides, i + 2 + base_sides, i + 1 + base_sides));
        faces.push_back(Face(1, i + 2, i + 1));
    }
    // two last faces
    faces.push_back(Face(base_sides + 1, 1, base_sides));
    //faces.push_back(Face(1, base_sides + 1, base_sides * 2 - 1));


    prism.processRawVerts(verts, verts, faces);
    prism.fitBoundingBox();

    return prism;
}

PrimMeshModel PrimMeshModel::Sphere(vec3 p, GLfloat radius, int subdivisions)
{
    PrimMeshModel sphere;
    sphere.name = "Sphere";

    std::vector<vec3> verts;
    std::vector<Face> faces;

    // create 12 vertices of a icosahedron
    const GLfloat t = (1.0 + std::sqrt(5.0)) / 2.0;

    verts.push_back(p + radius * normalize(vec3(-1, t, 0)));
    verts.push_back(p + radius * normalize(vec3(1, t, 0)));
    verts.push_back(p + radius * normalize(vec3(-1, -t, 0)));
    verts.push_back(p + radius * normalize(vec3(1, -t, 0)));

    verts.push_back(p + radius * normalize(vec3(0, -1, t)));
    verts.push_back(p + radius * normalize(vec3(0, 1, t)));
    verts.push_back(p + radius * normalize(vec3(0, -1, -t)));
    verts.push_back(p + radius * normalize(vec3(0, 1, -t)));

    verts.push_back(p + radius * normalize(vec3(t, 0, -1)));
    verts.push_back(p + radius * normalize(vec3(t, 0, 1)));
    verts.push_back(p + radius * normalize(vec3(-t, 0, -1)));
    verts.push_back(p + radius * normalize(vec3(-t, 0, 1)));

    // 5 faces around point 0
    faces.push_back(Face(1, 12, 6));
    faces.push_back(Face(1, 6, 2));
    faces.push_back(Face(1, 2, 8));
    faces.push_back(Face(1, 8, 11));
    faces.push_back(Face(1, 11, 12));

    // 5 adjacent faces
    faces.push_back(Face(2, 6, 10));
    faces.push_back(Face(6, 12, 5));
    faces.push_back(Face(12, 11, 3));
    faces.push_back(Face(11, 8, 7));
    faces.push_back(Face(8, 2, 9));

    // 5 faces around point 3
    faces.push_back(Face(4, 10, 5));
    faces.push_back(Face(4, 5, 3));
    faces.push_back(Face(4, 3, 7));
    faces.push_back(Face(4, 7, 9));
    faces.push_back(Face(4, 9, 10));

    // 5 adjacent faces
    faces.push_back(Face(5, 10, 6));
    faces.push_back(Face(3, 5, 12));
    faces.push_back(Face(7, 3, 11));
    faces.push_back(Face(9, 7, 8));
    faces.push_back(Face(10, 9, 2));

    // refine triangles
    for (int i = 0; i < subdivisions; i++)
    {
        std::vector<Face> _faces;
        for (const auto& face : faces)
        {
            vec3 face_verts[3] = { verts[face.v[0] - 1], verts[face.v[1] - 1], verts[face.v[2] - 1] };
            int n = verts.size() + 1;
            for (int j = 0; j < 3; j++) {
                vec3 face_mid = (face_verts[j] + face_verts[(j + 1) % 3]) / 2;
                // project to sphere
                face_mid = radius * normalize(face_mid);
                verts.push_back(face_mid);
            }
            _faces.push_back(Face(face.v[0], n, n + 2));
            _faces.push_back(Face(face.v[1], n + 1, n));
            _faces.push_back(Face(face.v[2], n + 2, n + 1));
            _faces.push_back(Face(n, n + 1, n + 2));
        }
        faces = _faces;
    }

    sphere.processRawVerts(verts, verts, faces);
    sphere.fitBoundingBox();

    return sphere;
}