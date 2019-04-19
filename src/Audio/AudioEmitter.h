#pragma once

#include "ECS.h"

#include "SFML\Audio.hpp"

#include "Input\Input.h"

#include <iostream>

// IMPORTANT: SFML Supports a maximum of 256 sounds playing simultaneously
class AudioEmitter : public Component {
private:
	sf::SoundBuffer buffer;
	sf::Sound sound;
public:
	bool loop = false;
	float volume = 100;

	// Empty constructor
	AudioEmitter() = default;

	AudioEmitter(const std::string path)
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
			Play();
		}
	}

	void Play() 
	{
		//sound.setPosition(transform->position.x, transform->position.y, transform->position.z);
		sound.play();
	}

	void Stop() 
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

	/*
	void DrawUI() 
	{
		if (ImGui::SliderFloat("Volume", &volume, 0, 100)) 
		{
			setVolume(volume);
		}
	}
	*/

	void setVolume(float val) { sound.setVolume(val); }
	void setLooping(bool val) { loop = val; }
};