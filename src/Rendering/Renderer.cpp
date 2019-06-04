#include "Rendering\Renderer.h"

#include "Resources\ResourceManager.h"

using namespace Graphyte;

std::map<std::string, std::vector<Renderer*>> Renderer::renderers;

Renderer::Renderer()
{
}

void Renderer::SetMaterial(Material material)
{
	// Remove it from the map first
	renderers[materials[0].name].erase(std::remove(renderers[materials[0].name].begin(), renderers[materials[0].name].end(), this), renderers[materials[0].name].end());

	if (materials.size() == 0) 
	{
		materials.push_back(material);
	}
	else {
		materials[0] = material;
	}
	// Add it to list
	renderers[material.name].push_back(this);
}

void Renderer::RenderAllGrouped(Camera& camera)
{
	// GROUP MATERIALS
	for (auto const&[key, val] : renderers)
	{
		ResourceManager::GetMaterial(key).Use();

		// FOREACH 
		for (int i = 0; i < val.size(); i++) {
			if (val[i]->enabled)
				val[i]->Render(camera);
		}
	}
}

void Renderer::RenderAllDepth(Camera& camera)
{
	for (auto const&[key, val] : renderers)
	{
		for (int i = 0; i < val.size(); i++) {
			if (val[i]->enabled)
				val[i]->Render(camera);
		}
	}
}

void Renderer::RenderAllWithShader(Camera& camera, Material& mat)
{
	mat.Use();
	for (auto const&[key, val] : renderers)
	{
		for (int i = 0; i < val.size(); i++) {
			if (val[i]->enabled)
				val[i]->Render(camera);
		}
	}
}