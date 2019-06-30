#include "ECS.h"

#include "imgui.h"

#include "Graphyte\Graphyte.h"

#include <iostream>

#include <Physics\Collider.h>

using namespace Graphyte;

void GameObject::Update() const
{
	if (enabled) {
		for (auto& child : children) 
		{
			child->Update();
		}

		for (auto& component : components)
		{
			if (component->enabled)
				component->Update();
		}
	}
}

void GameObject::LateUpdate() const
{
	if (enabled) {
		for (auto& child : children)
		{
			child->LateUpdate();
		}

		for (auto& component : components)
		{
			if (component->enabled)
				component->LateUpdate();
		}
	}
}

void GameObject::FixedUpdate() const
{
	if (enabled) {
		for (auto& child : children) 
		{
			child->FixedUpdate();
		}

		for (auto& component : components)
		{
			if (component->enabled)
				component->FixedUpdate();
		}
	}
}

void GameObject::OnSceneLoad() const
{
	if (enabled) {
		for (auto& child : children)
		{
			child->OnSceneLoad();
		}

		for (auto& component : components)
		{
			if (component->enabled)
				component->OnSceneLoad();
		}
	}
}

void GameObject::OnCollisionEnter(Collider& collider) 
{
	//std::cout << transform.name << " collision enter: " << collider.transform->name << "\n";
	for (auto& component : components)
	{
		component->OnCollisionEnter(collider);
	}
}

void GameObject::OnCollisionStay(Collider& collider)
{
	//std::cout << transform.name << " collision stay: " << collider.transform->name << "\n";
	for (auto& component : components)
	{
		component->OnCollisionStay(collider);
	}
}

void GameObject::OnCollisionExit(Collider& collider)
{
	//std::cout << transform.name << " collision exit: " << collider.transform->name << "\n";
	for (auto& component : components)
	{
		component->OnCollisionExit(collider);
	}
}

void GameObject::DrawComponents()
{
	ImGui::Text("Behaviour count: ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(componentCount).c_str());

	static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
	int node_clicked = -1;                // Temporary storage of what node we have clicked to process selection at the end of the loop. May be a pointer to your own node type, etc.
	ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3); // Increase spacing to differentiate leaves from expanded contents.

	for (int i = 0; i < components.size(); ++i)
	{
		// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selection_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0);
		// Node
		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, components[i]->name().c_str(), i);
		if (ImGui::IsItemClicked()) {
			node_clicked = i;
		}
		if (node_open)
		{
			components[i]->DrawUI();
			ImGui::TreePop();
		}
	}

	if (node_clicked != -1)
	{
		// Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
		if (ImGui::GetIO().KeyCtrl)
			selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
		else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
			selection_mask = (1 << node_clicked);           // Click to single-select
	}
	ImGui::PopStyleVar();
}

void GameObject::DrawChildren()
{
	ImGui::Text("Children count: ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(childCount).c_str());

	static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
	int node_clicked = -1;                // Temporary storage of what node we have clicked to process selection at the end of the loop. May be a pointer to your own node type, etc.

	ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3); // Increase spacing to differentiate leaves from expanded contents.
	for (int i = 0; i < children.size(); i++)
	{
		// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selection_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0);
		// Node
		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, children[i]->transform.name.data(), i);
		if (ImGui::IsItemClicked()) {
			GraphyteEditor::selectedGameObject = children[i];
			node_clicked = i;
		}
		if (node_open)
		{
			children[i]->DrawChildren();
			ImGui::TreePop();
		}
	}
	if (node_clicked != -1)
	{
		// Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
		if (ImGui::GetIO().KeyCtrl)
			selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
		else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
			selection_mask = (1 << node_clicked);           // Click to single-select
	}
	ImGui::PopStyleVar();
}

void GameObject::AddChild(GameObject* gameObject)
{
	gameObject->transform.parent = &transform;
	children.push_back(gameObject);

	++childCount;
}

/*
void GameObject::RemoveChild(int childIndex)
{
	children.erase(children.begin() + childIndex);
	--childCount;
}

void GameObject::RemoveBehaviour(std::string name)
{
	std::string temp = "class " + name;
	for (int i = 0; i < behaviourCount; i++) {
		if (behaviour[i]->name() == temp) {
			behaviour.erase(behaviour.begin() + i);
			--behaviourCount;
			break;
		}
	}
}
*/
