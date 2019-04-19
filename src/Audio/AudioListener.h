#pragma once

#include "ECS.h"

//#include "SFML\Audio\Sound.hpp"
#include "SFML\Audio\Listener.hpp"

class AudioListener : public Component {
public:
	AudioListener() = default;

	void Update() 
	{
		sf::Listener::setPosition(transform->position.x, transform->position.y, transform->position.y);

		Vector3 forwardDirection = transform->getForwardVector();
		sf::Listener::setDirection(forwardDirection.x, forwardDirection.y, forwardDirection.z);
	}
};