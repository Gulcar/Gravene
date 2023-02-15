#include "Text.h"

#include "Renderer.h"
#include <fmt/core.h>

void Text::Init(const std::string& fontFile, glm::vec2 filePixelSize, glm::vec2 charPixelSize)
{
	m_fontTexture = Renderer::LoadTexture(fontFile);
	m_filePixelSize = filePixelSize;
	m_charPixelSize = charPixelSize;
}

void Text::Write(std::string_view text, glm::vec2 pos, float size, bool centered, bool worldSpace, glm::vec3 color)
{
	glm::vec2 actualSize = { size * m_charPixelSize.x / m_charPixelSize.y, size };

	if (centered)
	{
		pos.x -= actualSize.x * (text.length() - 1) / 2.0f;
	}

	glm::vec2 currentPos = pos;

	if (worldSpace == false)
		currentPos += Renderer::GetCameraPos();

	for (int i = 0; i < text.length(); i++)
	{
		char character = text[i];

		if (character == '\n')
		{
			currentPos.x = pos.x;
			currentPos.y -= actualSize.y;
			continue;
		}
		if (character == ' ')
		{
			currentPos.x += actualSize.x;
			continue;
		}
		
		int xOffset = (character - 32) * m_charPixelSize.x;
		int yOffset = (xOffset / (int)m_filePixelSize.x) * m_charPixelSize.y;
		xOffset %= (int)m_filePixelSize.x;

		Renderer::DrawPartial(m_fontTexture, currentPos, actualSize, {xOffset, yOffset}, m_charPixelSize, m_filePixelSize, 0.0f, color);
		currentPos.x += actualSize.x;
	}
}

void Text::WriteRightAligned(std::string_view text, glm::vec2 pos, float size, bool worldSpace, glm::vec3 color)
{
	float charWidth = size * m_charPixelSize.x / m_charPixelSize.y;
	pos.x -= charWidth * text.length();
	Write(text, pos, size, false, worldSpace, color);
}

void Text::WriteFps(float deltaTime)
{
	static float updateFpsCounter = 0.15f;
	static std::string fpsString = fmt::format("{}fps", (int)(1.0f / deltaTime));
	updateFpsCounter -= deltaTime;
	if (updateFpsCounter < 0.0f)
	{
		fpsString = fmt::format("{}fps", (int)(1.0f / deltaTime));
		updateFpsCounter = 0.15f;
	}
	Text::Write(fpsString, { -Renderer::GetRightEdgeWorldPos() + 0.5f, 9.4f }, 0.8f);
}

uint32_t Text::m_fontTexture;
glm::vec2 Text::m_filePixelSize;
glm::vec2 Text::m_charPixelSize;
