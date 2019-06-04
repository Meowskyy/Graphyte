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

	class DirectionalLightLoc
	{
	public:
		GLuint Color;
		GLuint AmbientIntensity;
		GLuint Direction;
		GLuint DiffuseIntensity;
	};

	enum LightMode {
		Directional,
		Spot,
		Point
	};

	class Light : public Component {
	public:
		LightMode mode;
		Vector3 direction;
		Vector3 color;
		float ambientIntensity;
		float diffuseIntensity;

		void DrawUI()
		{
			ImGui::ColorEdit3("Light Color", (float*)&color); // Edit 3 floats representing a color
			ImGui::DragFloat3("Ambient Intensity", (float*)&direction);
			ImGui::SliderFloat("Ambient Intensity", &ambientIntensity, 0.0f, 2.0f);
			ImGui::SliderFloat("Diffuse Intensity", &diffuseIntensity, 0.0f, 2.0f);
		}
	};

	class Lighting {
	private:
		struct {
			GLuint Color;
			GLuint AmbientIntensity;
			GLuint DiffuseIntensity;
			GLuint Position;
			struct
			{
				GLuint Constant;
				GLuint Linear;
				GLuint Exp;
			} Atten;
		} m_pointLightsLocation[1];

		DirectionalLightLoc m_dirLightLocation;
	public:
		Lighting();

		virtual bool Init();

		void SetTextureUnit(unsigned int TextureUnit);

		void SetDirectionalLight(const Light& Light);
		void SetPointLights(unsigned int NumLights, const PointLight* pLights);
	};
}
