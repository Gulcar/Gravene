#pragma once

#include <cstdint>
#include <string>
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

	static uint32_t LoadTexture(const std::string& filename, int* outWidth = nullptr, int* outHeight = nullptr);
	static void Draw(uint32_t textureId, glm::vec2 pos, glm::vec2 size, float rotation = 0.0f, Rgb color = {1.0f, 1.0f, 1.0f});

	static struct GLFWwindow* GetWindow();

	static void CheckGlErrors();

	static void Destroy();

private:

	static uint32_t CreateShaderProgram(const char* vertexSource, const char* fragmentSource);
	static uint32_t CreateShader(const char* source, uint32_t type);

	static void CreateRectBuffers();

private:
	static std::unordered_map<std::string, uint32_t> m_loadedTextures;

	static struct GLFWwindow* m_window;
	static uint32_t m_defaultShader;

	static uint32_t m_rectVao;
	static uint32_t m_rectVbo;
	static uint32_t m_rectEbo;
};