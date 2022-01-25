#pragma once

#include <vector>
#include "PrimTypes.h"
#include "vec.h"

struct Material {
    Color ambient, diffuse, specular, emissive;

    float roughness, shininess;

    bool marble = false;

public:
    Material(Color ambient, Color diffuse, Color specular, Color emissive, float roughness, float shininess)
        : ambient(ambient), diffuse(diffuse), specular(specular), emissive(emissive), roughness(roughness), shininess(shininess) { }

    static Material* DefaultMaterial() {
        return new Material(1, 1, Color(1, 0, 0), 0, 0.5, 2);
    }
};
