#include "PlayerMovement.h"

#include "Physics\Rigidbody.h"

using namespace Graphyte;

void PlayerMovement::movement()
{
	Vector3 mov = Vector3(Input::GetAxis("Horizontal"), 0, Input::GetAxis("Vertical"));
	rb->AddForce(mov);
}
