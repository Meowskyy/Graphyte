#include "Rigidbody.h"

#include "Timers.h"

Vector3 Rigidbody::ComputeForces() {
	return Vector3(0, mass * -9.81, 0);
}

void Rigidbody::FixedUpdate()
{
	Vector3 force = ComputeForces();
	Vector3 acceleration = Vector3 (force.x / mass, force.y / mass, force.z / mass);

	velocity.x += acceleration.x * Time::getFixedDeltaTime();
	velocity.y += acceleration.y * Time::getFixedDeltaTime();
	velocity.z += acceleration.z * Time::getFixedDeltaTime();

	if (collidingObjects.size() > 0) 
	{
		velocity.y = 0;
	}

	transform->position.x += velocity.x * Time::getFixedDeltaTime();
	transform->position.y += velocity.y * Time::getFixedDeltaTime();
	transform->position.z += velocity.z * Time::getFixedDeltaTime();

	collidingObjects.clear();
}
