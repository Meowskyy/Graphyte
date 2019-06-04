#pragma once

#include "ECS.h"

#include <math.h>

#include "Time\Timers.h"

namespace Graphyte {
	class MoveAround : public Component {
	public:
		int i = 0;
		float amount = 2;
		float speed = 0.01f;

		float x = 0;
		float rotSpeed = 0.001f;

		float distance = 1.0f;

		void Update() {
			//transform->position.y = sin(i * speed) * amount;

			transform->rotation = Quaternion(Vector3(0, i * rotSpeed, 0));
			//transform->Rotate(transform->getRightVector(), rotSpeed);

			if (Input::GetKeyDown(GLFW_KEY_C)) {
				transform->position = Physics::RaycastMousePosition(distance);
				//ExtraRenderer::DrawLine(transform->position, Physics::RaycastMousePosition(distance));
			}

			i++;
		}

		void DrawUI() {
			ImGui::DragFloat("Distance from camera", &distance);
		}
	};
}