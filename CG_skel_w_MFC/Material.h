#pragma once

#include "vec.h"
#include "PrimTypes.h"

struct Material {
    Color base;
    Color emissive;

    float ambient_reflect;
    float roughness;
    float shininess;

protected:
    virtual Material ComputeAt(const vec3 v, const vec3 min, const vec3 max) {
        return Material(base, emissive, ambient_reflect, roughness, shininess);
    }
public:
    Material(Color base, Color emissive, float ambient_reflect, float roughness, float shininess)
        : base(base), emissive(emissive), ambient_reflect(ambient_reflect), roughness(roughness), shininess(shininess) { }

    std::vector<Material> ComputeAt(const vec3 v[3], const vec3 min, const vec3 max) {
        std::vector<Material> res;
        for (int i = 0; i < 3; i++)
            res.push_back(ComputeAt(v[i], min, max));
        return res;
    }

    static Material* DefaultMaterial() {
        return new Material(Color(0.5), Color(0), 0.5, 0.5, 0.5);
    }
};

inline Material InterpolateMaterial(std::vector<Material> mats, const float a, const float b, const float c) {
    return Material(mats[0].base * a + mats[1].base * b + mats[2].base * c,
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
        : Material(Color(), Color(), ambient_reflect, roughness, shininess) { } // no base color or any emissive color!
protected:
    Material ComputeAt(const vec3 v, const vec3 min, const vec3 max) {
        const float angle = (v.x - min.x) / (max.x - min.x);
        const Color color(max(0, abs(3 * angle - 1.5) - 0.5), max(0, 1 - abs(3 * angle - 1)), max(0, 1 - abs(3 * angle - 2)));
        return Material{ color, Color(), ambient_reflect, roughness, shininess };
    }
};
