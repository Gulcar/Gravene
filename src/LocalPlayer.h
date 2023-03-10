#pragma once

#include <glm/vec2.hpp>

class LocalPlayer
{
public:
	LocalPlayer();

	void Update(float deltaTime);
	void Draw();

	void SetRandPos();

	void GivePowerup();

	glm::vec2 Position = { 0.0f, 0.0f };
	float Rotation = 0.0f;
	glm::vec2 Scale = { 2.0f, 2.0f };

private:
	uint32_t m_texture;

	float m_moveSpeed = 9.0f;

	float m_dashSpeed = 50.0f;
	float m_dashTime = 0.15f;
	float m_dashCooldown = 1.0f;
	float m_timeSinceDash = 10.0f;
	
	float m_speedPowerUpTime = -1.0f;
	float m_doubleShootPowerUpTime = -1.0f;
	float m_tripleShootPowerUpTime = -1.0f;
};
