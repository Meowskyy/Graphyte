#pragma once

#include "ECS.h"

#include "Resources\ResourceManager.h"
#include "Rendering\Camera.h"

#include "Physics\UniformGrid.h"

#include "Rendering\ShadowMap.h"

namespace Graphyte {
	class Lighting;
	class Light;

	class Scene {
	private:
		ShadowMap m_shadowMap;
		Lighting* m_lighting;
		Light* light;
	public:
		static std::vector<GameObject*> gameObjects;
		static UniformGrid uniformGrid;

		float updateTimer;

		Scene() = default;
		~Scene();

		// UPDATING COMPONENTS
		void OnSceneLoad();	// Run on scene load?
		void Update();		// Run after every frame
		void LateUpdate();	// Run at the end of every frame
		void FixedUpdate(); // Called multiple times during physics checks
		void CheckCollisions(); // Check collisions for each gameObject once each frame

		// RENDERING OBJECTS
		void Render(Camera& camera);		// Render meshes on screen
		void RenderDepth(Camera& camera);
		void RenderObjects(Camera& camera);

		// TODO: All these are temporary
		void AddGameObject();
		void AddCrysisGameObject();
		void AddChild();
		void Add1000GameObjects();
		void AddWorld();
		void AddGrid();
		void AddGridTestGameObject();
		
		// SHAPES
		void AddPlane();
		void AddCube();
		void AddSphere();

		void AddLight();
		void AddCameraObject();

		static GameObject* Instantiate(GameObject* original);
		static GameObject* Instantiate(GameObject* original, GameObject& parent);
		static GameObject* Instantiate(GameObject* original, const Vector3& position);
		static void Destroy(GameObject* original);
		static void DestroyLast();

		std::vector<GameObject*> GetAllRootObjects() const;
	};
}