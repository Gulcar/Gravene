#pragma once

#include <cstdint>
#include <string_view>
#include <glm/vec2.hpp>
#include <unordered_map>

struct Rgb
{
	float r, g, b;
};

class Renderer
{
public:

	static void Init();

	static uint32_t LoadImage(std::string_view filename);
	static void Draw(uint32_t imageId, glm::vec2 pos, glm::vec2 size, float rotation = 0.0f, Rgb color = {0.0f, 0.0f, 0.0f});

	static struct GLFWwindow* GetWindow();

	static void CheckGlErrors();

	static void Destroy();

private:

	static uint32_t CreateShaderProgram(const char* vertexSource, const char* fragmentSource);
	static uint32_t CreateShader(const char* source, uint32_t type);

private:
	static std::unordered_map<std::string, uint32_t> m_loadedImages;
	static struct GLFWwindow* m_window;
	static uint32_t m_defaultShader;
};