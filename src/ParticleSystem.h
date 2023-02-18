#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

class ParticleSystem
{
private:
	struct Particle
	{
		glm::vec2 pos;
		glm::vec2 velocity;
		glm::vec3 color;
		float size;
	};

public:
	ParticleSystem(int particleCount, glm::vec2 pos, glm::vec3 color, float speed, float time, float particleSize);

	void Update(float deltaTime);
	void Draw();

	inline bool IsDone() { return m_time < 0.0f; }

private:
	std::vector<Particle> m_particles;
	float m_sizeChange;
	float m_time;

	static uint32_t m_pixelTexture;
};
