#pragma once

#include "vec.h"
#include "mat.h"

class Entity
{
	mat4 transform;
public:
	Entity();
	virtual ~Entity() = 0;

	void setTransform(const mat4& _transform);
	void move(vec3 v);
	void setPosition(vec3 v);
	void rotate(int axis, float degree);
	void scale(vec3 v);

	vec3 getPosition();
};

