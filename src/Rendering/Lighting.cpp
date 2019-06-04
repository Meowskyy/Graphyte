#include "Lighting.h"

#include "Resources\ResourceManager.h"

using namespace Graphyte;

Lighting::Lighting()
{
}

bool Lighting::Init()
{
	return false;
}

void Lighting::SetDirectionalLight(const Light& Light)
{
	Shader& shader = ResourceManager::GetShader("Standard");
	shader.SetVector3f("lightPos", Light.transform->position);
	shader.SetFloat("ambientStrength", Light.ambientIntensity);
	//Vector3 Direction = Light.direction;
	//Direction = glm::normalize(Direction);
	shader.SetVector3f("lightColor", Light.color);
	shader.SetVector3f("objectColor", Vector3(0.6f, 0, 0));
	//shader.SetFloat("gDirectionalLight.Base.Color", Light.diffuseIntensity);
}

void Lighting::SetPointLights(unsigned int NumLights, const PointLight* pLights)
{
	Shader &shader = ResourceManager::GetShader("Standard");
	shader.SetInteger("gNumPointLights", NumLights);

	for (unsigned int i = 0; i < NumLights; i++) {
		shader.SetVector3f("gPointLights[%d].Base.Color", pLights[i].Color);
		shader.SetFloat("gPointLights[%d].Base.AmbientIntensity", pLights[i].AmbientIntensity);
		shader.SetFloat("gPointLights[%d].Base.DiffuseIntensity", pLights[i].DiffuseIntensity);
		shader.SetVector3f("gPointLights[%d].Base.Position", pLights[i].Position);
		shader.SetFloat("gPointLights[%d].Base.Atten.Constant", pLights[i].Attenuation.Constant);
		shader.SetFloat("gPointLights[%d].Base.Atten.Linear", pLights[i].Attenuation.Linear);
		shader.SetFloat("gPointLights[%d].Base.Atten.Exp", pLights[i].Attenuation.Exp);
	}
}
