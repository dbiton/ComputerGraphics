#include "stdafx.h"
#include "Entity.h"

Entity::Entity()
{
}

Entity::~Entity()
{
}

void Entity::setTransform(mat4 _transform) noexcept
{
    transform = _transform;
}

mat4 Entity::getTransform() const
{
    return transform;
}

void Entity::setPosition(vec3 v) noexcept
{
    pos = v;
}

vec3 Entity::getPosition() const
{
    return pos;
}

void Entity::setRotation(vec3 r) noexcept
{
    rot = r;
}

vec3 Entity::getRotation() const
{
    return rot;
}

void Entity::setScale(vec3 s) noexcept
{
    scale = s;
}

vec3 Entity::getScale() const
{
    return scale;
}

void Entity::moveBy(vec3 v) noexcept
{
    pos += v;
}

void Entity::rotateBy(vec3 r) noexcept
{
    rot += r; // TODO check if this is right
}

void Entity::scaleBy(vec3 v) noexcept
{
    scale *= v;
}
