#include "LocalPlayer.h"

#include "Renderer.h"
#include "Input.h"
#include <glm/gtx/compatibility.hpp>
#include <fmt/core.h>
#include "Network.h"
#include <glm/gtc/random.hpp>

LocalPlayer::LocalPlayer()
{
    m_texture = Renderer::LoadTexture("resources/player.png");
}

void LocalPlayer::Update(float deltaTime)
{
    glm::vec2 mousePos = Input::GetMouseWorldPos();
    glm::vec2 dir = mousePos - Position;
    Rotation = std::atan2(dir.y, dir.x);
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

    float speed = (m_timeSinceDash < m_dashTime) ? m_dashSpeed : m_moveSpeed;
    if (m_speedPowerUpTime > 0.0f) speed *= 1.4f;

    Position += move * deltaTime * speed;

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

    if (Input::GetMouseButtonDown(0))
    {
        glm::vec2 bulletDir = glm::normalize(dir);

        bool doubleShoot = m_doubleShootPowerUpTime > 0.0f;
        bool tripleShoot = m_tripleShootPowerUpTime > 0.0f;

        if (!doubleShoot)
        {
            Network::SendShoot(Position + bulletDir, bulletDir);
        }
        if (doubleShoot || tripleShoot)
        {
            float rot;

            rot = Rotation - 20.0f;
            bulletDir = { cosf(rot / 180.0f * 3.14159265359f), sinf(rot / 180.0f * 3.14159265359f) };
            Network::SendShoot(Position + bulletDir, bulletDir);

            rot = Rotation + 20.0f;
            bulletDir = { cosf(rot / 180.0f * 3.14159265359f), sinf(rot / 180.0f * 3.14159265359f) };
            Network::SendShoot(Position + bulletDir, bulletDir);
        }
    }

    m_speedPowerUpTime -= deltaTime;
    m_doubleShootPowerUpTime -= deltaTime;
    m_tripleShootPowerUpTime -= deltaTime;
}

void LocalPlayer::Draw()
{
    Renderer::Draw(m_texture, Position, Scale, Rotation, { 0.067f, 0.341f, 0.941f });
}

void LocalPlayer::SetRandPos()
{
    Position.x = rand() % 91 - 45;
    Position.y = rand() % 51 - 25;
}

void LocalPlayer::GivePowerup()
{
    int r = rand() % 3;
    switch (r)
    {
    case 0: m_speedPowerUpTime = 10.0f; break;
    case 1: m_doubleShootPowerUpTime = 10.0f; break;
    case 2: m_tripleShootPowerUpTime = 10.0f; break;
    }
}

