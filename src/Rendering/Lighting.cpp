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
	Shader& shader = ResourceManager::GetShader("Standard").Use();
	shader.SetVector3f("directionalLight.Direction", Light.transform->GetForwardVector());
	shader.SetFloat("directionalLight.Base.AmbientIntensity", Light.ambientIntensity);
	shader.SetFloat("directionalLight.Base.DiffuseIntensity", Light.diffuseIntensity);
	shader.SetVector3f("directionalLight.Base.Color", Light.color);
	shader.SetVector3f("directionalLight.Base.Position", Light.transform->position);
	shader.SetFloat("directionalLight.Base.Bias", Light.bias);
	shader.SetFloat("directionalLight.Base.NormalBias", Light.normalBias);
}

void Lighting::SetPointLights(unsigned int NumLights, const PointLight* pLights)
{
	Shader &shader = ResourceManager::GetShader("Standard").Use();
	shader.SetInteger("numPointLights", NumLights);

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
