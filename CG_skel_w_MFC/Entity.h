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

	void setTransform(mat4 _transform) noexcept;
	mat4 getTransform() const;

	void setPosition(vec3 v) noexcept;
	vec3 getPosition() const;

	void setRotation(vec3 r) noexcept;	// euler angles, radians
	vec3 getRotation() const;			// euler angles, radians

	void setScale(vec3 s) noexcept;
	vec3 getScale() const;

	void moveBy(vec3 v) noexcept;
	void rotateBy(vec3 r) noexcept;		// euler angles, radians
	void scaleBy(vec3 v) noexcept;
};

