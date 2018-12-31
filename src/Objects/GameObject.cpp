#include "GameObject.h"


GameObject::GameObject()
{
	//transform.name = "GameObject";

	transform.position = glm::vec3(0, 0, 0);
	transform.scale = glm::vec3(1, 1, 1);
	transform.rotation = glm::quat(glm::vec3(0, 0, 0));
}

GameObject::~GameObject()
{
	std::cout << "GameObject deleted" << std::endl;
}

void GameObject::AddBehaviour(BehaviourScript *script)
{
	behaviour.push_back(script);
	behaviour[behaviour.size() - 1]->OnScriptAdd();
}

template <class T>
T GameObject::GetBehaviour()
{
	for (int i = 0; i < behaviour.size; i++) {
		if (typeid(behaviour[i]) == typeid(type)) {
			return dynamic_cast<T>(behaviour[i]);
		}
	}
	return null;
}

template <class T>
T GameObject::GetBehaviourByUUID(int UUID)
{
	for (int i = 0; i < behaviour.size; i++) {
		if (behaviour[i].UUID == UUID) {
			return behaviour[i];
		}
	}
	return null;
}
