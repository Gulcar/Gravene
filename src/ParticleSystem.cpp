#include "ParticleSystem.h"

#include <glm/gtc/random.hpp>
#include "Renderer.h"

ParticleSystem::ParticleSystem(int particleCount, glm::vec2 pos, glm::vec3 color, float speed, float time, float particleSize)
    : m_particles(particleCount)
{
    for (auto& particle : m_particles)
    {
        particle.pos = pos;

        glm::vec2 dir = glm::normalize(glm::circularRand(1.0f));
        float randomSpeed = glm::linearRand(speed * 0.9f, speed * 1.1f);
        particle.velocity = dir * randomSpeed;

        particle.color = glm::linearRand(color * 0.9f, color * 1.1f);

        particle.size = glm::linearRand(particleSize * 0.9f, particleSize * 1.1f);
    }

    if (m_pixelTexture == -1)
        m_pixelTexture = Renderer::LoadTexture("resources/white_pixel.png");

    m_sizeChange = particleSize / time;
    m_time = time * 1.15f;
}

void ParticleSystem::Update(float deltaTime)
{
    for (auto& particle : m_particles)
    {
        particle.pos += particle.velocity * deltaTime;
        particle.size -= m_sizeChange * deltaTime;
    }

    m_time -= deltaTime;
}

void ParticleSystem::Draw()
{
    for (auto& particle : m_particles)
    {
        if (particle.size > 0.0f)
            Renderer::Draw(m_pixelTexture, particle.pos, { particle.size, particle.size }, 0.0f, particle.color);
    }
}

uint32_t ParticleSystem::m_pixelTexture = -1;
