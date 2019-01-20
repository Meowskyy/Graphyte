#pragma once

#include <memory>
#include <vector>
#include <bitset>
#include <array>

#include "Physics\Physics3D.h"
#include "BoundingBox.h"

#include "imgui.h"

class GameObject;
class Component;

using ComponentID = std::size_t;

inline ComponentID GetComponentTypeID() 
{
	static ComponentID lastID = 0;
	return lastID++;
}

template <typename T> inline ComponentID GetComponentTypeID() noexcept
{
	static ComponentID typeID = GetComponentTypeID();
	return typeID;
}

constexpr std::size_t maxComponents = 32;

using ComponentBitSet = std::bitset<maxComponents>;
using ComponentArray = std::array<Component*, maxComponents>;

class Transform {
private:
	Vector3 eulerAngles;

	Vector3 oldPosition = Vector3(0, 0, 0);
	Quaternion oldRotation;
	Vector3 oldScale = Vector3(1, 1, 1);

public:
	std::string name = "GameObject";
	
	Transform* parent; // Parent transform
	GameObject* gameObject; // THE GAMEOBJECT THIS BELONGS TO

	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
	Quaternion rotation;
	Vector3 scale = Vector3(1, 1, 1);

	BoundingBox boundingBox;

	Transform() { }

	// Transform directions
	Vector3 forward = getForwardVector();
	Vector3 right = getRightVector();
	Vector3 up = getUpVector();

	// Transform direction methods
	Vector3 getForwardVector();
	Vector3 getRightVector();
	Vector3 getUpVector();

	// Position Parent + Local
	Vector3 GetWorldPosition();
	//glm::vec3 GetWorldRotation();
	//glm::vec3 GetWorldScale();

	Transform* GetChild(int index);

	const bool positionHasChanged()
	{
		bool hasChanged = position != oldPosition;

		oldPosition = position;
		return hasChanged;
	}

	const bool rotationHasChanged()
	{
		bool hasChanged = rotation != oldRotation;

		oldRotation = rotation;
		return hasChanged;
	}

	const bool scaleHasChanged()
	{
		bool hasChanged = scale != oldScale;

		oldScale != scale;
		return hasChanged;
	}

	void Rotate(const Vector3 direction, const float speed);
	void setParent(Transform& transform);
};

class Component {
public:
	bool enabled = true;

	GameObject* gameObject;
	Transform* transform;

	Component() 
	{
		transform = new Transform();
		//gameObject = new GameObject();
	}

	virtual ~Component() 
	{
		delete transform;
		//delete gameObject;
	}

	virtual std::string name() { return typeid(*this).name(); }
	virtual operator std::string() const { return typeid(*this).name(); }

	// Functions
	// Called after the scene loads
	virtual void OnSceneLoad() {}
	// Called once the script is enabled after being disabled
	virtual void OnEnable() {}
	// Called once the script is added
	virtual void OnComponentAdded() {}
	// Called every physics iteration
	virtual void FixedUpdate() {}
	// Called every frame
	virtual void Update() {}

	// Functions : Physics
	// Called every time the collider hits something
	virtual void OnCollisionEnter(GameObject& gameObject) {}
	virtual void OnCollisionStay(GameObject& gameObject) {}
	virtual void OnCollisionExit(GameObject& gameObject) {}

	virtual void DrawUI() {}	// Draws info about the script, only if in _DEBUG is defined
};

class GameObject {
private:
	std::vector<std::unique_ptr<Component>> components;

	ComponentArray componentArray;
	ComponentBitSet componentBitSet;

public:
	Transform transform = Transform();

	std::vector<GameObject*> children;
	std::vector<GameObject*> collisionList;

	bool hasCollision = true;

	bool enabled = true;

	int componentCount = 0;
	int childCount = 0;

	void Update();
	void FixedUpdate();
	void OnSceneLoad();

	void OnCollisionEnter(GameObject& gameObject);
	void CheckCollisions();
	void OnCollisionExit(GameObject& gameObject);

	void DrawComponents();
	void DrawChildren();

	void AddChild(GameObject& gameObject);

	// Adds a components of type T to the object
	template<typename T, typename... TArgs>
	T& AddComponent(TArgs &&... mArgs)
	{
		T* component(new T(std::forward<TArgs>(mArgs)...));
		component->transform = &transform;
		component->gameObject = this;

		std::unique_ptr<Component> uPtr{ component };
		components.emplace_back(std::move(uPtr));

		componentArray[GetComponentTypeID<T>()] = component;
		componentBitSet[GetComponentTypeID<T>()] = true;

		component->OnComponentAdded();

		++componentCount;

		return *component;
	}

	// Get the first components that has the same name
	template<typename T> T& GetComponent() const
	{
		auto ptr(componentArray[GetComponentTypeID<T>()]);

		return *static_cast<T*>(ptr);
	}
};