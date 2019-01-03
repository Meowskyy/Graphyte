#include "GameObject.h"
#include "Scripting\BehaviourScript.h"

#include "imgui.h"
#include "Graphyte\Graphyte.h"

void GameObject::Update()
{
	if (enabled) {
		for (int i = 0; i < childCount; i++) {
			children[i]->Update();
		}

		for (int behaviourIndex = 0; behaviourIndex < behaviourCount; behaviourIndex++)
		{
			behaviour.at(behaviourIndex)->Update();
		}
	}
}

void GameObject::FixedUpdate()
{
	if (enabled) {
		for (int i = 0; i < childCount; i++) {
			children[i]->FixedUpdate();
		}

		for (int behaviourIndex = 0; behaviourIndex < behaviourCount; behaviourIndex++)
		{
			behaviour.at(behaviourIndex)->FixedUpdate();
		}
	}
}

void GameObject::OnSceneLoad()
{
	if (enabled) {
		for (int i = 0; i < childCount; i++) {
			children[i]->OnSceneLoad();
		}

		for (int behaviourIndex = 0; behaviourIndex < behaviourCount; behaviourIndex++)
		{
			behaviour.at(behaviourIndex)->OnSceneLoad();
		}
	}
}

void GameObject::OnRigidbodyCollisionEnter() 
{
	for (int behaviourIndex = 0; behaviourIndex < behaviourCount; behaviourIndex++)
	{
		//behaviour.at(behaviourIndex)->OnRigidbodyCollisionEnter();
	}
}

void GameObject::DrawBehaviours()
{
	ImGui::Text("Behaviour count: ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(behaviourCount).c_str());

	static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
	int node_clicked = -1;                // Temporary storage of what node we have clicked to process selection at the end of the loop. May be a pointer to your own node type, etc.
	ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3); // Increase spacing to differentiate leaves from expanded contents.

	for (int i = 0; i < behaviour.size(); i++)
	{
		// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selection_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0);
		// Node
		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, behaviour[i]->name().c_str(), i);
		if (ImGui::IsItemClicked()) {
			node_clicked = i;
		}
		if (node_open)
		{
			behaviour[i]->DrawUI();
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
			Graphyte::selectedGameObject = children[i];
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

void GameObject::AddChild(GameObject* object)
{
	object->transform.parent = &transform;
	children.push_back(object);

	++childCount;
}

void GameObject::RemoveChild(int childIndex)
{
	children.erase(children.begin() + childIndex);
	--childCount;
}

BehaviourScript* GameObject::AddBehaviour(BehaviourScript* script)
{
	script->transform = &transform;
	script->gameObject = this;
	script->OnBehaviourAdded();

	behaviour.push_back(script);

	if (behaviourDict.count(script->name()) == 0) {
		behaviourDict[script->name()] = behaviourCount;
	}

	return behaviour[++behaviourCount - 1];
}

BehaviourScript* GameObject::GetBehaviour(std::string name)
{
	if (behaviourDict.count("class " + name) != 0)
	{
		return behaviour[behaviourDict["class " + name]];
	}

	for (int i = 0; i < behaviourCount; i++) {
		if (behaviour[i]->name() == "class " + name) {
			return behaviour[i];
		}
	}

	return nullptr;
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
