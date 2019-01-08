#pragma once

#include "Objects\Transform.h"

#include "imgui.h"

#include <iostream>
#include <map>

class BehaviourScript {
public:
	// Is the BehaviourScript active
	bool enabled = true;

	// The transform this script belongs to
	Transform* transform;

	// The gameObject this script belongs to
	GameObject* gameObject;

	// Constructor
	BehaviourScript();
	virtual ~BehaviourScript();

	virtual std::string name() { return typeid(*this).name(); }
	virtual operator std::string() const { return typeid(*this).name(); }

	// Functions
	// Called after the scene loads
	virtual void OnSceneLoad() {}
	// Called once the script is enabled after being disabled
	virtual void OnEnable() {}
	// Called once the script is added
	virtual void OnBehaviourAdded() {}
	// Called every physics iteration
	virtual void FixedUpdate() {}
	// Called every frame
	virtual void Update() {}

	// Functions : Physics
	// Called every time the collider hits something
	virtual void OnRigidbodyCollisionEnter() {}

	virtual void DrawUI() {}			// Draws info about the script, only if in _DEBUG is defined
};