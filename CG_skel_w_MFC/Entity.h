#pragma once

#include "vec.h"
#include "mat.h"

// An entity is anything that exists in our scene and can be moved - i.e. camera, model, light, etc
class Entity
{
	mat4 transform;
	vec3 pos, rot, scale;
public:
	Entity();
	virtual ~Entity();

	void setTransform(const mat4 &_transform);
	const mat4& getTransform() const;

	void setPosition(const vec3 &v);
	vec3 getPosition() const;

	void setRotation(const vec3 &r);	// euler angles, radians
	vec3 getRotation() const;	// euler angles, radians

	void setScale(const vec3 &s);
	vec3 getScale() const;

	void moveBy(const vec3 &v);
	void rotateBy(const vec3 &r);		// eular angles, radians
	void scaleBy(const vec3 &s);
};

