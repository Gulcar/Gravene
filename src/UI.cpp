#include "UI.h"

#include <glm/vec2.hpp>
#include "Renderer.h"
#include "Text.h"
#include "Input.h"
#include <fmt/core.h>

UI::Button::Button()
{
	m_texture = Renderer::LoadTexture("resources/white_pixel.png");
}

void UI::Button::Update()
{
	glm::vec2 mousePos = Input::GetMouseWorldPos();
	if (mousePos.x < m_pos.x + m_size.x / 2.0f &&
		mousePos.x > m_pos.x - m_size.x / 2.0f &&
		mousePos.y < m_pos.y + m_size.y / 2.0f &&
		mousePos.y > m_pos.y - m_size.y / 2.0f)
	{
		m_isHovering = true;
	}
	else
	{
		m_isHovering = false;
	}

	if (m_isHovering && Input::GetMouseButtonDown(0))
		m_clicked = true;
	else
		m_clicked = false;
}

void UI::Button::Draw()
{
	if (m_isHovering)
		Renderer::Draw(m_texture, m_pos, m_size, 0.0f, glm::vec3(0.2f));
	else
		Renderer::Draw(m_texture, m_pos, m_size, 0.0f, glm::vec3(0.15f));

	Text::Write(m_text, m_pos, m_size.y * 0.8f, true);
}


UI::InputField::InputField()
	: m_text("")
{
	m_texture = Renderer::LoadTexture("resources/white_pixel.png");
	glfwSetCharCallback(Renderer::GetWindow(), (GLFWcharfun)UI::InputField::GlfwCharCallback);
}

UI::InputField::~InputField()
{
	if (s_currentInputField == this)
		s_currentInputField = nullptr;
}

void UI::InputField::Update()
{
	glm::vec2 mousePos = Input::GetMouseWorldPos();
	if (mousePos.x < m_pos.x + m_size.x / 2.0f &&
		mousePos.x > m_pos.x - m_size.x / 2.0f &&
		mousePos.y < m_pos.y + m_size.y / 2.0f &&
		mousePos.y > m_pos.y - m_size.y / 2.0f)
	{
		m_isHovering = true;
	}
	else
	{
		m_isHovering = false;
	}

	if (Input::GetMouseButtonDown(0))
	{
		if (m_isHovering && !m_isActive)
		{
			m_isActive = true;
			s_currentInputField = this;
		}
		else if (!m_isHovering)
		{
			m_isActive = false;
		}
	}

	if (m_isActive && Input::GetKeyDown(GLFW_KEY_BACKSPACE) && m_text.length() > 0)
	{
		if (Input::GetKey(GLFW_KEY_LEFT_CONTROL))
			m_text = "";
		else
			m_text.pop_back();
	}
}

void UI::InputField::Draw()
{
	if (m_isActive)
		Renderer::Draw(m_texture, m_pos, m_size, 0.0f, glm::vec3(0.3f));
	else if (m_isHovering)
		Renderer::Draw(m_texture, m_pos, m_size, 0.0f, glm::vec3(0.2f));
	else
		Renderer::Draw(m_texture, m_pos, m_size, 0.0f, glm::vec3(0.15f));

	Text::Write(m_text, m_pos, m_size.y * 0.8f, true);
}

void UI::InputField::GlfwCharCallback(GLFWwindow* window, unsigned int codepoint)
{
	if (s_currentInputField == nullptr) return;
	if (s_currentInputField->m_isActive == false) return;

	if (codepoint >= 0 && codepoint <= 128)
		s_currentInputField->m_text += (char)codepoint;
}

UI::InputField* UI::InputField::s_currentInputField = nullptr;
