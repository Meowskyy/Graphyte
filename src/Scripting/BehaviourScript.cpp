#include "Scripting\BehaviourScript.h"

#include "Objects\GameObject.h"

BehaviourScript::BehaviourScript()
{
	transform = new Transform();
	gameObject = new GameObject();
}

BehaviourScript::~BehaviourScript()
{
	delete transform;
	delete gameObject;
}