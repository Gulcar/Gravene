#include "LocalPlayer.h"

#include "Renderer.h"
#include "Input.h"
#include <glm/gtx/compatibility.hpp>
#include <fmt/core.h>

LocalPlayer::LocalPlayer()
{
	m_texture = Renderer::LoadTexture("resources/player.png");
}

void LocalPlayer::Update(float deltaTime)
{
	glm::vec2 mousePos = Input::GetMouseWorldPos();
	glm::vec2 dir = mousePos - Position;
	Rotation = std::atan2f(dir.y, dir.x);
	Rotation *= 180.0f / 3.14159265359f;


	glm::vec2 move = {0.0f, 0.0f};

	if (Input::GetKey(GLFW_KEY_A))
		move.x -= 1.0f;
	if (Input::GetKey(GLFW_KEY_D))
		move.x += 1.0f;
	if (Input::GetKey(GLFW_KEY_W))
		move.y += 1.0f;
	if (Input::GetKey(GLFW_KEY_S))
		move.y -= 1.0f;

	if (move.x != 0.0f || move.y != 0.0f)
		move = glm::normalize(move);

	static glm::vec2 prevMove = { 0.0f, 0.0f };
	if (move.x == 0.0f && move.y == 0.0f)
		move = prevMove * 0.05f;
	else
		prevMove = move;

	if (m_timeSinceDash < m_dashTime)
		Position += move * deltaTime * m_dashSpeed;
	else
		Position += move * deltaTime * m_moveSpeed;

	m_timeSinceDash += deltaTime;

	if (m_timeSinceDash > m_dashTime + m_dashCooldown && Input::GetKeyDown(GLFW_KEY_SPACE))
		m_timeSinceDash = 0.0f;

	// bounds
	if (Position.x > 48.0f)
	{
		Position.x = 48.0f;
		prevMove.x = -prevMove.x;
	}
	else if (Position.x < -48.0f)
	{
		Position.x = -48.0f;
		prevMove.x = -prevMove.x;
	}
	if (Position.y > 27.0f)
	{
		Position.y = 27.0f;
		prevMove.y = -prevMove.y;
	}
	else if (Position.y < -27.0f)
	{
		Position.y = -27.0f;
		prevMove.y = -prevMove.y;
	}

	Renderer::SetCameraPos(glm::lerp(this->Position, mousePos, 0.1f));
}

void LocalPlayer::Draw()
{
	Renderer::Draw(m_texture, Position, Scale, Rotation, { 1.0f, 0.2f, 0.2f });
}
