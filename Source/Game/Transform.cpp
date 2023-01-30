#include "pch.h"
#include "Transform.h"

Transform::Transform(Entity*& myEntity)
	: Component::Component(myEntity)
{
}

Transform::~Transform()
{
}

void Transform::Start()
{
}
