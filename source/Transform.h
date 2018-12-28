#pragma once

#include "glm\glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

#include <string>


#define _USE_MATH_DEFINES
#include <math.h>

#include <vector>
#include <iostream>

#define USE_GLM_QUATERNION

const float PI = 3.141592653589793;

#ifndef USE_GLM_QUATERNION
class Quaternion {
private:
	glm::vec3 euler;

	void updateAngles() 
	{
		euler = glm::eulerAngles(rotation);
		eulerAngles = glm::eulerAngles(rotation) * 180.0f / PI;
		//eulerAngles.x += 180;
		//eulerAngles.y += 180;
	}

public:
	glm::vec3 eulerAngles; // Returns angles in 360 degrees
	glm::quat rotation;

	Quaternion(float yaw = 0, float roll = 0, float pitch = 0)
	{

	}

	// for Vec3 angles
	Quaternion(glm::vec3 angles)
	{
		rotation = glm::quat(glm::vec3(angles.y / 180 * M_PI, angles.x / 180 * M_PI, angles.z));
	}

	Quaternion operator=(Quaternion &quat) 
	{
		quat.updateAngles();

		this->eulerAngles = quat.eulerAngles;
		//std::cout << quat.eulerAngles.x << std::endl;

		return *this;
	}
};
#endif

class Transform {
private:
	glm::vec3 euler;

public:
	std::string name = "GameObject";
	Transform *parent;
	std::vector<Transform*> children;

	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::quat rotation;
	glm::vec3 scale = glm::vec3(1, 1, 1);

	Transform() {};

	void DrawUI();

	// Transform directions
	glm::vec3 forward = getForwardVector();
	glm::vec3 right = getRightVector();
	glm::vec3 up = getUpVector();

	// Transform direction methods
	glm::vec3 getForwardVector();
	glm::vec3 getRightVector();
	glm::vec3 getUpVector();

	void Rotate(glm::vec3 direction, float speed);
	void setParent(Transform &transform);
};