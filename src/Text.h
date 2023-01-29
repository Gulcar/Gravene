#pragma once

#include <string_view>
#include <glm/vec2.hpp>
#include "Renderer.h"

class Text
{
public:
	static void Init(const std::string& fontFile, glm::vec2 filePixelSize, glm::vec2 charPixelSize);
	static void Write(std::string_view text, glm::vec2 pos, float size, bool centered = false, bool worldSpace = false, glm::vec3 color = {1.0f, 1.0f, 1.0f});

	static void WriteFps(float deltaTime);

private:
	static uint32_t m_fontTexture;
	static glm::vec2 m_filePixelSize;
	static glm::vec2 m_charPixelSize;
};
