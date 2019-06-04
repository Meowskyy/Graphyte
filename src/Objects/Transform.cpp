#include "ECS.h"

using namespace Graphyte;

Vector3 Transform::getForwardVector() const
{
	Vector3 front = rotation * Vector3(0, 0, 1);
	front = glm::normalize(front);

	return front;
}

Vector3 Transform::getRightVector() const
{
	return glm::normalize(glm::cross(getForwardVector(), Vector3(0, 1, 0)));
}

Vector3 Transform::getUpVector() const
{
	return glm::normalize(glm::cross(getRightVector(), getForwardVector()));
}

// TODO: Fixing this error in modelLoader
Vector3 Transform::GetWorldPosition() const
{
	//std::cout << name << std::endl;
	//return (parent != nullptr) ? parent->position + position : position;

	Vector3 pos = position;
	
	if (scale.x != 0 && position.x != 0) {
		pos.x = position.x / scale.x;
	}

	if (scale.y != 0 && position.y != 0) {
		pos.y = position.y / scale.y;
	}

	if (scale.z != 0 && position.z != 0) {
		pos.z = position.z / scale.z;
	}

	//return (parent == nullptr) ? pos : parent->GetWorldPosition() + pos;
	return pos;
}

Transform* Transform::GetChild(const int index) const
{
	return &gameObject->children[index]->transform;
}

void Transform::Rotate(const Vector3 direction, const float speed)
{
	rotation *= Quaternion(direction * speed);
}

void Transform::setParent(Transform &transform)
{
	this->parent = &transform;
}