#include "Transform.h"

#include "imgui.h"
#include "Graphyte.h"

glm::vec3 Transform::getForwardVector()
{
	// Calculate the new Front vector
	glm::vec3 front = rotation * glm::vec3(0, 0, 1);
	// Normalizing it
	front = glm::normalize(front);

	return front;
}

glm::vec3 Transform::getRightVector()
{
	return glm::normalize(glm::cross(getForwardVector(), glm::vec3(0, 1, 0)));
}

glm::vec3 Transform::getUpVector()
{
	return glm::normalize(glm::cross(getRightVector(), getForwardVector()));
}

void Transform::Rotate(glm::vec3 direction, float speed) {}

void Transform::setParent(Transform &transform)
{
	this->parent = &transform;
}


static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
void Transform::DrawUI() {
	int node_clicked = -1;

	ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3); // Increase spacing to differentiate leaves from expanded contents.
	for (int i = 0; i < children.size(); i++)
	{
		// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selection_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0);
		// Node
		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, children[i]->name.data(), i);
		if (ImGui::IsItemClicked()) {
			//Graphyte::selectedGameObject = children[i]->gameObject;
			node_clicked = i;
		}
		if (node_open)
		{
			children[i]->DrawUI();
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