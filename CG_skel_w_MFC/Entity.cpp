#include "stdafx.h"
#include "Entity.h"

Entity::Entity()
{
}

Entity::~Entity()
{
}

void Entity::setTransform(const mat4& _transform)
{
	transform = _transform;
}

const mat4& Entity::getTransform() const
{
	return transform;
}

void Entity::setPosition(const vec3& v)
{
	transform[0][3] = v.x;
	transform[1][3] = v.y;
	transform[2][3] = v.z;
}

vec3 Entity::getPosition() const
{
	return vec3(transform[0][3], transform[1][3], transform[2][3]);
}


void Entity::setRotation(const vec3& r)
{
	vec3 _r = getRotation();
	rotateBy(r-_r);
}

vec3 Entity::getRotation() const
{
	vec3 s = getScale();
	return vec3(); // BUG
}

void Entity::setScale(const vec3& s)
{
	vec3 _s = getScale();
	scaleBy(vec3(s.x / _s.x, s.y / _s.y, s.z / _s.z));
}

vec3 Entity::getScale() const
{
	return vec3(length(transform[0]), length(transform[1]), length(transform[2]));
}


void Entity::moveBy(const vec3& v)
{
	transform[0][3] += v.x;
	transform[1][3] += v.y;
	transform[2][3] += v.z;
}

void Entity::rotateBy(const vec3& r)
{
	transform = RotateZ(r.z) * RotateY(r.y) * RotateX(r.x) * transform;
}

void Entity::scaleBy(const vec3& s)
{
	transform = Scale(s) * transform;
}

// TODO probably better to separately save the position and rotations, and only construct the transformation on demand. rotateBy and scaleBy seem like they could cause weird behaviour as they are now, and getRotation seems too complex to bother as it is. we could also add a sort of "updated" flag which lets us do some lazy stuff with the transformation
