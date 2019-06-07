#pragma once

#include "Math\Math.h"

#include "glad\glad.h"

#include "ECS.h"

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
		float nearClipPlane = 0.1f, farClipPlane = 10;
		LightMode mode = Directional;
		Vector3 direction;
		Vector3 color = Vector3(1, 1, 1);
		float ambientIntensity = 0.5f, diffuseIntensity = 0.5f;

		void DrawUI()
		{
			ImGui::DragFloat("Size", &size);

			ImGui::DragFloat("Near Clip", &nearClipPlane);
			ImGui::DragFloat("Far Clip", &farClipPlane);

			ImGui::ColorEdit3("Light Color", (float*)&color);
			ImGui::DragFloat3("Ambient Intensity", (float*)&direction);
			ImGui::SliderFloat("Ambient Intensity", &ambientIntensity, 0.0f, 1.0f);
			ImGui::SliderFloat("Diffuse Intensity", &diffuseIntensity, 0.0f, 1.0f);
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
