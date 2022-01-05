#pragma once

#include <vector>
#include "PrimTypes.h"
#include "vec.h"

struct Material {
    Color diffuse, specular, emissive;

    float ambient_reflect, roughness, shininess;

protected:
    virtual Material ComputeAt(const vec3 v, const vec3 min, const vec3 max) {
        return Material(diffuse, specular, emissive, ambient_reflect, roughness, shininess);
    }
public:
    Material(Color diffuse, Color specular, Color emissive, float ambient_reflect, float roughness, float shininess)
        : diffuse(diffuse), specular(specular), emissive(emissive), ambient_reflect(ambient_reflect), roughness(roughness), shininess(shininess) { }

    std::vector<Material> ComputeAt(const vec3 v[3], const vec3 min, const vec3 max) {
        std::vector<Material> res;
        for (int i = 0; i < 3; i++)
            res.push_back(ComputeAt(v[i], min, max));
        return res;
    }

    static Material* DefaultMaterial() {
        return new Material(Color(1), Color(1), Color(0), 1, 0.5, 2);
    }
};

inline Material InterpolateMaterial(std::vector<Material> mats, const float a, const float b, const float c) {
    return Material(mats[0].diffuse * a + mats[1].diffuse * b + mats[2].diffuse * c,
        mats[0].specular * a + mats[1].specular * b + mats[2].specular * c,
        mats[0].emissive * a + mats[1].emissive * b + mats[2].emissive * c,
        mats[0].ambient_reflect * a + mats[1].ambient_reflect * b + mats[2].ambient_reflect * c,
        mats[0].roughness * a + mats[1].roughness * b + mats[2].roughness * c,
        mats[0].shininess * a + mats[1].shininess * b + mats[2].shininess * c
    );
}

inline Material AverageMaterial(std::vector<Material> mats) {
    return InterpolateMaterial(mats, 1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0);
}

struct FullSatSpectrumMaterial : public Material {
    FullSatSpectrumMaterial(float ambient_reflect, float roughness, float shininess)
        : Material(Color(), Color(), Color(), ambient_reflect, roughness, shininess) { } // no base color or any emissive color!
protected:
    Material ComputeAt(const vec3 v, const vec3 min, const vec3 max) {
        const float angle = (v.x - min.x) / (max.x - min.x);
        const Color color(max(0, abs(3 * angle - 1.5) - 0.5), max(0, 1 - abs(3 * angle - 1)), max(0, 1 - abs(3 * angle - 2)));
        return Material{ color, color, Color(), ambient_reflect, roughness, shininess };
    }
};

struct PhysSpectrumMaterial : public Material {
    PhysSpectrumMaterial(Color diffuse, Color specular)
        : Material(diffuse, specular, Color(), 0, 0, 0) { }
protected:
    Material ComputeAt(const vec3 v, const vec3 min, const vec3 max) {
        const float angle = (v.x - min.x) / (max.x - min.x);
        const Color color(max(0, abs(3 * angle - 1.5) - 0.5), max(0, 1 - abs(3 * angle - 1)), max(0, 1 - abs(3 * angle - 2)));
        return Material{ diffuse, specular, Color(), color.x, color.y, color.z };
    }
};
