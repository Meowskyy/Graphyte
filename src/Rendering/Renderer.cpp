#include "Rendering\Renderer.h"

#include "Resources\ResourceManager.h"

#include <iostream>

#include "Camera.h"

#include "ShadowMap.h"

using namespace Graphyte;

std::map<std::string, std::vector<Renderer*>> Renderer::renderers;

Renderer::Renderer(){}

// TODO: Sorting transparent materials to the end of the map
void Renderer::SetMaterial(Material material)
{
	std::cout << "Adding " << transform->name << " to renderers list\n";
	std::cout << "Adding material " << material.shader.shaderName << " to list\n";

	if (materials.size() == 0) 
	{
		materials.push_back(material);
	}
	else {
		// Remove it from the map first
		renderers[materials[0].name].erase(std::remove(renderers[materials[0].name].begin(), renderers[materials[0].name].end(), this), renderers[materials[0].name].end());

		materials[0] = material;
	}
	// Add it to list
	renderers[material.name].push_back(this);
}

void Renderer::RenderAllGrouped(Camera& camera)
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// GROUP MATERIALS
	for (auto const&[materialName, renderer] : renderers)
	{
		ResourceManager::GetMaterial(materialName).Use();
		ResourceManager::GetMaterial(materialName).shader.SetMatrix4("view", camera.GetViewMatrix());

		ResourceManager::currentShader = ResourceManager::GetMaterial(materialName).shader;

		// FOREACH 
		for (int i = 0; i < renderer.size(); i++) {
			if (renderer[i]->enabled)
				renderer[i]->Render(camera);
		}
	}
}

void Renderer::RenderAllDepth(Camera& camera)
{
	for (auto const&[key, val] : renderers)
	{
		for (int i = 0; i < val.size(); i++) {
			if (val[i]->enabled) 
			{
				Transform& transf = *val[i]->transform;
				transf.boundingBox.Recalculate();
				bool isVisible = camera.IsTransformInView(transf);
				isVisible = true;

				if (isVisible) 
				{
					val[i]->Render(camera);
				}
			}
		}
	}
}

void Renderer::RenderAllWithShader(Camera& camera, Material& mat)
{
	mat.Use();
	mat.shader.SetMatrix4("view", camera.GetViewMatrix());

	for (auto const&[key, val] : renderers)
	{
		for (int i = 0; i < val.size(); i++) {
			if (val[i]->enabled)
				val[i]->Render(camera);
		}
	}
}