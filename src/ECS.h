#pragma once

#include <memory>
#include <vector>
#include <bitset>
#include <array>
#include <chrono>
#include <map>

#include "Physics\Physics3D.h"
#include "Physics\BoundingBox.h"

#include "imgui.h"

namespace Graphyte {

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
		GameObject* gameObject; // The GameObject this transform belongs to

		Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
		Quaternion rotation;
		Vector3 scale = Vector3(1, 1, 1);

		AxisAlignedBoundingBox boundingBox;

		Transform() : position(Vector3(0.0f, 0.0f, 0.0f)), scale(Vector3(1, 1, 1)), boundingBox(this) {}

		Matrix4 GetTransformMatrix() const;
		Matrix4 GetRotScaleMatrix() const;

		// Transform direction methods
		Vector3 GetForwardVector() const;
		Vector3 GetRightVector() const;
		Vector3 GetUpVector() const;

		// Position Parent + Local
		Vector3 GetWorldPosition() const;
		// Vector3 GetWorldRotation() const;
		// Vector3 GetWorldScale() const;

		Transform* GetChild(const int index) const;

		inline bool positionHasChanged()
		{
			bool hasChanged = position != oldPosition;

			oldPosition = position;
			return hasChanged;
		}

		inline bool rotationHasChanged()
		{
			bool hasChanged = rotation != oldRotation;

			oldRotation = rotation;
			return hasChanged;
		}

		inline bool scaleHasChanged()
		{
			bool hasChanged = scale != oldScale;

			oldScale != scale;
			return hasChanged;
		}

		void Rotate(const Vector3 direction, const float speed);
		void SetParent(Transform& transform);
	};

	class Component {
	private:

	public:
		bool enabled = true;

		GameObject* gameObject;
		Transform* transform;

		Component() : transform(new Transform()) {}

		virtual ~Component()
		{
			if (transform != nullptr)
			{
				delete transform;
			}

			transform = nullptr;
		}

		virtual std::string name() { return typeid(*this).name(); }
		virtual operator std::string() const { return typeid(*this).name(); }

		// Functions
		// Called after the scene loads
		virtual void OnSceneLoad() {}
		// Called once the component is enabled after being disabled
		virtual void OnEnable() {}
		// Called once the component is added
		virtual void OnComponentAdded() {}
		// Called every physics iteration
		virtual void FixedUpdate() {}
		// Called every frame
		virtual void Update() {}
		// Called at the end of every frame
		virtual void LateUpdate() {}

		// Functions : Physics
		// Called every time the collider hits something
		virtual void OnCollisionEnter(Collider& collider) {}
		virtual void OnCollisionStay(Collider& collider) {}
		virtual void OnCollisionExit(Collider& collider) {}

		virtual void DrawUI() {}	// Draws info about the component, only if in _DEBUG is defined
	};

	class GameObject {
	private:
		std::vector<std::unique_ptr<Component>> components;

		ComponentArray componentArray;
		ComponentBitSet componentBitSet;

	public:
		Transform transform = Transform();

		std::vector<GameObject*> children;
		std::vector<Collider*> collisionList;

		~GameObject()
		{
			for (auto& child : children)
			{
				delete child;
			}

			for (auto& component : components)
			{
				component->transform = nullptr;
				component->gameObject = nullptr;
			}
		}

		bool enabled = true;

		int componentCount = 0;
		int childCount = 0;

		// TODO: Is this even necessary? Can't we just store all components somewhere and then call them without this?
		void FixedUpdate() const;
		void Update() const;
		void LateUpdate() const;
		void OnSceneLoad() const;

		void OnCollisionEnter(Collider& collider);
		void OnCollisionStay(Collider& collider);
		void OnCollisionExit(Collider& collider);
		// SCRIPT PART END

		void DrawComponents();
		void DrawChildren();

		void AddChild(GameObject* gameObject);

		// Adds a components of type T to the object
		template<typename T, typename... TArgs>
		T& AddComponent(TArgs &&... mArgs)
		{
			T* component(new T(std::forward<TArgs>(mArgs)...));
			//T* component(new T(transform));
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
}
