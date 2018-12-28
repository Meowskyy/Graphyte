#pragma once

#include "BehaviourScript.h"

//#include "SFML\Audio\Sound.hpp"
#include "SFML\Audio\Listener.hpp"

class AudioListener : public BehaviourScript {
public:
	AudioListener(Transform &transform) : BehaviourScript(transform) 
	{
		std::cout << "Setup audio listener" << std::endl;
		sf::Listener::setPosition(transform.position.x, transform.position.y, transform.position.y);

		// TODO: Add this as a separate option
		sf::Listener::setGlobalVolume(50.f);
	}

	void Update() 
	{
		sf::Listener::setPosition(transform.position.x, transform.position.y, transform.position.y);

		glm::vec3 forwardDirection = transform.getForwardVector();
		sf::Listener::setDirection(forwardDirection.x, forwardDirection.y, forwardDirection.z); // TODO: AudioListener rotation
	}
};