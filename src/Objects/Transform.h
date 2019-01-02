#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

#include <string>

#define _USE_MATH_DEFINES
#include <math.h>

#include <vector>
#include <iostream>

#include "Math\Vector2.h"
#include "Math\Vector3.h"
#include "Math\Quaternion.h"

#include "BoundingBox.h"

class GameObject;

class Transform {
private:
	Vector3 euler;

	Vector3 oldPosition = Vector3(0, 0, 0);
	Quaternion oldRotation;
	Vector3 oldScale = Vector3(1, 1, 1);

public:
	std::string name = "GameObject";
	Transform* parent;

	GameObject* gameObject; // THE GAMEOBJECT THIS BELONGS TO

	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
	Quaternion rotation;
	Vector3 scale = Vector3(1, 1, 1);

	BoundingBox boundingBox;

	Transform() {}

	// Transform directions
	Vector3 forward = getForwardVector();
	Vector3 right = getRightVector();
	Vector3 up = getUpVector();

	// Transform direction methods
	Vector3 getForwardVector();
	Vector3 getRightVector();
	Vector3 getUpVector();

	// Position Parent + Local
	Vector3 GetWorldPosition();
	//glm::vec3 GetWorldRotation();
	//glm::vec3 GetWorldScale();

	Transform* GetChild(int index);

	bool positionHasChanged() 
	{ 
		bool hasChanged = position != oldPosition; 

#ifdef _DEBUG
		if (hasChanged) {
			//std::cout << "Position Changed" << std::endl;
		}
#endif

		oldPosition = position;
		return hasChanged;
	}

	bool rotationHasChanged() 
	{ 
		bool hasChanged = rotation != oldRotation; 

#ifdef _DEBUG
		if (hasChanged) {
			//std::cout << "Rotation Changed" << std::endl;
		}
#endif

		oldRotation = rotation;
		return hasChanged;
	}

	bool scaleHasChanged() 
	{ 
		bool hasChanged = scale != oldScale;

#ifdef _DEBUG
		if (hasChanged) {
			//std::cout << "Scale Changed" << std::endl;
		}
#endif

		oldScale != scale;
		return hasChanged;
	}

	void Rotate(Vector3 direction, float speed);
	void setParent(Transform &transform);
};