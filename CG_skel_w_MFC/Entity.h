#pragma once

#include "vec.h"
#include "mat.h"

// An entity is anything that exists in our scene and can be moved - i.e. camera, model, light, etc
class Entity
{
	mat4 transform;
public:
	Entity();
	virtual ~Entity() = 0;

	void setTransform(mat4 _transform);
	mat4 getTransform() const;

	void setPosition(vec3 v);
	vec3 getPosition() const;

	void setRotation(vec3 r);	// euler angles, radians
	vec3 getRotation() const;	// euler angles, radians

	void setScale(vec3 s);
	vec3 getScale() const;

	void moveBy(vec3 v);
	void rotateBy(vec3 r);		// eular angles, radians
	void scaleBy(vec3 v);
};

