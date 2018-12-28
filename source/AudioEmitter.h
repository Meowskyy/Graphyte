#pragma once

#include "BehaviourScript.h"

#include "SFML\Audio.hpp"

#include "Input.h"

// IMPORTANT: SFML Supports a maximum of 256 sounds playing simultaneously
class AudioEmitter : public BehaviourScript {
private:
	sf::SoundBuffer buffer;
	sf::Sound sound;
public:
	bool loop = false;

	AudioEmitter(Transform &transform) : BehaviourScript(transform) {}
	AudioEmitter(Transform &transform, const std::string path) : BehaviourScript(transform) 
	{
		if (!buffer.loadFromFile(path)) {
			std::cout << "Error loading sound file: " << path << std::endl;
		}
		else {
			std::cout << "Loaded audio file: " << path << std::endl;
		}

		sound.setBuffer(buffer);
	}

	// Temporary
	void Update() {
		if (Input::GetKeyPressed(GLFW_KEY_1)) {
			play();
		}
	}

	void play() 
	{
		sound.setPosition(transform.position.x, transform.position.y, transform.position.z);
		sound.play();
	}

	void stop() 
	{
		sound.stop();
	}

	void setSoundClip(const std::string path) 
	{
		if (!buffer.loadFromFile(path)) {
			std::cout << "Error loading sound file: " << path << std::endl;
		}
		else {
			std::cout << "Loaded audio file: " << path << std::endl;
		}

		sound.setBuffer(buffer);
	}

	void setVolume(float val) { sound.setVolume(val); }
	void setLooping(bool val) { loop = val; }
};