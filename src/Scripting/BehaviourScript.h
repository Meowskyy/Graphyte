#pragma once

#include "Objects\Transform.h"

#include <iostream>

class BehaviourScript {
public:
	int UUID = 0; // TODO: Make this unique to every BehaviourScript
	Transform &transform;

	// Constructor
	BehaviourScript(Transform &newTransform) : transform(newTransform) {};
	~BehaviourScript() {};

	virtual std::string name() { return typeid(*this).name(); };

	// Functions
	virtual void OnSceneLoad() {}		// Called once the script is loaded/added
	virtual void FixedUpdate() {}	// TODO: Implementing FixedUpdate
	virtual void Update() {}		// Called every frame
	virtual void OnBehaviourAdded() {}	// Called once the script gets attached to a gameobject

	// TODO: Remove this and move it to AudioEmitter
	virtual void Play() {}; // For audio temporarily
};