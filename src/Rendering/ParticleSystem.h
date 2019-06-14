#pragma once

#include "Renderer.h"

namespace Graphyte {
	class Camera;

	struct Particle {
		Vector3 position, velocity;
		Vector4 color; // RGBA
		float life, size;

		Particle() : position(0.0f), velocity(0.0f), color(1.0f), life(-1.0f), size(1.0f) {}
	};

	class ParticleSystem : public Renderer {
	private:
		GLuint VAO, VBO;
		GLuint particles_position_buffer;	// The VBO containing the positions and sizes of the particles
		GLuint particles_color_buffer; 		// The VBO containing the colors of the particles

		std::vector<Vector4> g_particle_position_size_data;
		std::vector<Vector4> g_particle_color_data;

		int lastUsedParticle = 0;
		std::vector<Particle> particles;

		// Finds a Particle in ParticlesContainer which isn't used yet.
		// (i.e. life < 0);
		int FindUnusedParticle();

		int particleCount = 0;
	public:
		bool loop = true;
		bool isPlaying = true;
		int maxParticles = 1000;
		float scale = 1;
		float lifeTime = 5;
		float framerate = 0.016f;
		Vector4 tint = Vector4(1, 1, 1, 1);
		float alphaCutoff = 0.25f;

		void Init();
		void RespawnParticle(Particle& particle);
		void Update();
		void Render(Camera& camera);

		void DrawUI();
	};
}