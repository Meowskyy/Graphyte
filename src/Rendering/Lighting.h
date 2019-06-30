#pragma once

#include "Math\Math.h"

#include "glad\glad.h"

#include "ECS.h"

#include "Screen.h"
#include "Camera.h"

namespace Graphyte {
	struct BaseLight
	{
		Vector3 Color;
		float AmbientIntensity;
		float DiffuseIntensity;
	};

	struct PointLight : public BaseLight
	{
		Vector3 Position;

		struct
		{
			float Constant;
			float Linear;
			float Exp;
		} Attenuation;
	};

	enum LightMode {
		Directional,
		Spot,
		Point
	};

	class Light : public Component {
	public:
		float size = 10;
		float nearClipPlane = 0.3f, farClipPlane = 7.5f;
		LightMode mode = Directional;
		Vector3 direction;
		Vector3 color = Vector3(1, 1, 1);
		float ambientIntensity = 0.5f, diffuseIntensity = 0.5f;

		float bias = 0.05f;
		float normalBias = 0.005f;

		Matrix4 lightSpaceMatrix;

		Matrix4& GetLightSpaceMatrix()
		{
			Matrix4 lightProjection = glm::ortho(-size, size, -size, size, nearClipPlane, farClipPlane);
			//lightProjection = glm::perspective(70.0f, (float)Screen::width / (float)Screen::height, nearClipPlane, farClipPlane);

			Vector3 position = transform->position;
			Vector3 upVector = transform->GetUpVector();
			Vector3 forwardVector = transform->GetForwardVector();

			//Matrix4 lightView = (glm::translate(Matrix4(1.0), forwardVector));
			//Matrix4 lightView = glm::lookAt(position, position + forwardVector, upVector);

			Matrix4 lightView = glm::lookAt(transform->position, Vector3(0, 0, 0), Vector3(0, 1, 0));
			//Matrix4 lightView = glm::mat4_cast(-transform->rotation);

			lightSpaceMatrix = (lightProjection * lightView);

			return lightSpaceMatrix;
		}

		void DrawUI()
		{
			ImGui::DragFloat("Size", &size);

			ImGui::DragFloat("Near Clip", &nearClipPlane);
			ImGui::DragFloat("Far Clip", &farClipPlane);

			ImGui::ColorEdit3("Light Color", (float*)&color);
			ImGui::DragFloat3("Light Direction", (float*)&direction);
			ImGui::SliderFloat("Ambient Intensity", &ambientIntensity, 0.0f, 1.0f);
			ImGui::SliderFloat("Diffuse Intensity", &diffuseIntensity, 0.0f, 1.0f);

			ImGui::SliderFloat("Bias", &bias, 0.0f, 1.0f);
			ImGui::SliderFloat("Normal Bias", &normalBias, 0.0f, 1.0f);
		}

	};

	class Lighting {
	private:

	public:
		Lighting();

		virtual bool Init();

		void SetTextureUnit(unsigned int TextureUnit);

		void SetDirectionalLight(const Light& Light);
		void SetPointLights(unsigned int NumLights, const PointLight* pLights);
	};
}
