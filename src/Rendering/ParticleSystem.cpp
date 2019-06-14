#include "ParticleSystem.h"

#include "glad\glad.h"

#include "Time\Timers.h"

#include "Camera.h"

#include "Math\Math.h"

using namespace Graphyte;

void Graphyte::ParticleSystem::Init()
{
	std::cout << "Init ParticleSystem\n";

	// The VBO containing the 4 vertices of the particles.
	// Thanks to instancing, they will be shared by all particles.
	static const float vertices[] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 1.0f
	};

	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	// Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	// texture coord attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glGenBuffers(1, &particles_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(Vector4), NULL, GL_STREAM_DRAW); // Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glGenBuffers(1, &particles_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(Vector4), NULL, GL_STREAM_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(0);

	particles.reserve(maxParticles);
	g_particle_position_size_data.reserve(maxParticles);
	g_particle_color_data.reserve(maxParticles);

	for (int i = 0; i < maxParticles; ++i) 
	{
		particles.push_back(Particle());
		g_particle_position_size_data.push_back(Vector4(0, 0, 0, 1));
		g_particle_color_data.push_back(Vector4(1, 1, 1, 1));
	}
}

void Graphyte::ParticleSystem::RespawnParticle(Particle& particle)
{
	float randomX = ((rand() % 100) - 50) / 10.0f;
	float randomZ = ((rand() % 100) - 50) / 10.0f;

	particle.position = transform->position;
	particle.color = Vector4(1, 1, 1, 1.0f);
	particle.life = lifeTime;
	particle.velocity = Vector3(randomX, 15, randomZ);
}

int Graphyte::ParticleSystem::FindUnusedParticle()
{
	// Search from last used particle, this will usually return almost instantly
	for (int i = lastUsedParticle; i < maxParticles; ++i) {
		if (particles[i].life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}

	// Otherwise, do a linear search
	for (int i = 0; i < lastUsedParticle; ++i) {
		if (particles[i].life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}

	// Override first particle if all others are alive
	lastUsedParticle = 0;
	return 0;
}

void Graphyte::ParticleSystem::Update() 
{
	int newParticles = (int)(Time::deltaTime * maxParticles);

	if (newParticles > (int)(framerate * maxParticles))
	{
		newParticles = (int)(framerate * maxParticles);
	}

	std::map<float, Particle> sorted;
	particleCount = 0;
	for (int i = 0; i < maxParticles; ++i)
	{
		if (i < newParticles) {
			int unusedParticle = FindUnusedParticle();
			RespawnParticle(particles[unusedParticle]);
		}

		Particle& p = particles[i];
		p.life -= Time::deltaTime; // reduce life
		if (p.life > 0.0f)
		{	// particle is alive, thus update
			p.velocity -= Vector3(0, 9.81f, 0) * Time::deltaTime;
			p.position += p.velocity * Time::deltaTime;

			p.color.a = Math::lerp(0.0f, 1.0f, p.life / lifeTime); // FADE BY LIFETIME

			g_particle_position_size_data[particleCount] = Vector4(p.position, p.size); // XYZ = Position and W = Particle Size
			g_particle_color_data[particleCount] = Vector4(p.color); // RGBA

			float distance = glm::length(Camera::mainCamera->transform->position - p.position);
			sorted[distance] = p;

			particleCount++;
		}
		else 
		{
			p.life = 0.0f;
		}
	}

	particleCount = 0;
	for (std::map<float, Particle>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
	{
		g_particle_position_size_data[particleCount] = Vector4(it->second.position, it->second.size); // XYZ = Position and W = Particle Size
		particleCount++;
	}
}

void Graphyte::ParticleSystem::Render(Camera& camera)
{
	glDisable(GL_CULL_FACE);

	Matrix4 ModelMatrix = transform->GetTransformMatrix();

	materials[0].shader.SetMatrix4("model", ModelMatrix);
	materials[0].shader.SetVector4f("tintColor", tint);
	materials[0].shader.SetFloat("scale", scale);
	materials[0].shader.SetFloat("cutoff", alphaCutoff);

	//glDepthMask(false); //disable z-testing as alternative to sorting

	/*
	// STANDARD VERSION
	particleCount = 0;
	for (auto particle : particles) 
	{
		if (particle.life > 0.0f)
		{
			materials[0].shader.SetVector3f("offset", particle.position);
			materials[0].shader.SetVector4f("color", particle.color);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);
			particleCount++;
		}
	}
	*/

	/*
	// SORTED VERSION
	for (std::map<float, Particle>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
	{
		if (it->second.life > 0.0f)
		{
			materials[0].shader.SetVector3f("offset", it->second.position);
			materials[0].shader.SetVector4f("color", it->second.color);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			//glDrawArrays(GL_TRIANGLES, 0, 4);
			glBindVertexArray(0);
			particleCount++;
		}
	}
	*/

	// INSTANCED VERSION
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(Vector4), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * sizeof(Vector4), &g_particle_position_size_data[0]);

	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(Vector4), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * sizeof(Vector4), &g_particle_color_data[0]);

	glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
	glVertexAttribDivisor(2, 1); // positions : one per quad (its center) -> 1
	glVertexAttribDivisor(3, 1); // color : one per quad -> 1

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particleCount);

	glBindVertexArray(0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);

	//glEnable(GL_DEPTH_TEST);
	//glDepthMask(true); //enable z-testing
}

void Graphyte::ParticleSystem::DrawUI()
{
	ImGui::DragInt("Particle Count", &particleCount);
	ImGui::DragInt("Max Particles", &maxParticles);
	ImGui::DragFloat("Lifetime", &lifeTime);
	ImGui::DragFloat("Scale", &scale);
	ImGui::DragFloat("Framerate", &framerate);
	ImGui::ColorEdit4("Tint", (float*)&tint);
	ImGui::DragFloat("Cutoff", &alphaCutoff, 0.05f, 0.0f, 1.0f);
}
