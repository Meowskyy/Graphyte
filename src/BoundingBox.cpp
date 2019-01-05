#include "BoundingBox.h"

#include "Objects\Transform.h"

bool BoundingBox::Contains(BoundingBox & otherBoundingBox)
{
	if (this->min.x < otherBoundingBox.min.x &&
		this->min.y < otherBoundingBox.min.y &&
		this->min.z < otherBoundingBox.min.z &&
		this->max.x > otherBoundingBox.max.x &&
		this->max.y > otherBoundingBox.max.y &&
		this->max.z > otherBoundingBox.max.z
		)
	{
		return true;
	}

	return false;
}

bool BoundingBox::Contains(Transform& transform)
{
	//std::cout << "########" << std::endl;
	std::cout << "Min: " << transform.position.x + transform.boundingBox.min.x << ", " << transform.position.y + transform.boundingBox.min.y << ", " << transform.position.z + transform.boundingBox.min.z;
	std::cout << " / ";
	std::cout << this->min.x << this->min.y << this->min.z << std::endl;

	if (this->min.x <= transform.position.x + transform.boundingBox.min.x &&
		this->min.y <= transform.position.y + transform.boundingBox.min.y &&
		this->min.z <= transform.position.z + transform.boundingBox.min.z &&
		this->max.x >= transform.position.x + transform.boundingBox.max.x &&
		this->max.y >= transform.position.y + transform.boundingBox.max.y &&
		this->max.z >= transform.position.z + transform.boundingBox.max.z
		)
	{
		return true;
	}

	return false;
}
