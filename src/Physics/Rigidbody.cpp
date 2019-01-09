#include "Physics\Rigidbody.h"

#include "Time\Timers.h"

Vector3 Rigidbody::ComputeForces() {
	return Vector3(0, mass * -9.81, 0);
}

void Rigidbody::FixedUpdate()
{
	Vector3 force = ComputeForces();
	Vector3 acceleration = Vector3 (force.x / mass, force.y / mass, force.z / mass);

	velocity.x += acceleration.x * Time::fixedDeltaTime;
	velocity.y += acceleration.y * Time::fixedDeltaTime;
	velocity.z += acceleration.z * Time::fixedDeltaTime;

	if (gameObject->collisionList.size() > 0) 
	{
		velocity.y = 0;
	}

	transform->position.x += velocity.x * Time::fixedDeltaTime;
	transform->position.y += velocity.y * Time::fixedDeltaTime;
	transform->position.z += velocity.z * Time::fixedDeltaTime;
}
