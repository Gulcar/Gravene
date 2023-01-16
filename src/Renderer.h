#pragma once

#include <cstdint>
#include <string>
#include <glm/mat4x4.hpp>
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

	static uint32_t LoadTexture(const std::string& filename);
	static void Draw(uint32_t textureId, glm::vec2 pos, glm::vec2 size, float rotation = 0.0f, Rgb color = {1.0f, 1.0f, 1.0f});
	static void DrawPartial(uint32_t textureId, glm::vec2 pos, glm::vec2 size,
		glm::vec2 pixelPos, glm::vec2 pixelSize, glm::vec2 textureSize, float rotation = 0.0f, Rgb color = {1.0f, 1.0f, 1.0f});

	static void NewFrame();

	static inline struct GLFWwindow* GetWindow() { return m_window; }
	static inline int GetWindowWidth() { return m_windowWidth; }
	static inline int GetWindowHeight() { return m_windowHeight; }

	static void CheckGlErrors();

	static void CreateProjectionMat();

	static float GetRightEdgeWorldPos();

	static void Destroy();

private:

	static uint32_t CreateShaderProgram(const char* vertexSource, const char* fragmentSource);
	static uint32_t CreateShader(const char* source, uint32_t type);

	static void CreateRectBuffers();
	static void CreatePartialRectBuffers();

	static void GlfwFramebufferSizeCallback(struct GLFWwindow* window, int width, int height);
	static void GlfwErrorCallback(int errorCode, const char* desc);

private:
	static std::unordered_map<std::string, uint32_t> m_loadedTextures;

	static struct GLFWwindow* m_window;
	static uint32_t m_defaultShader;

	static uint32_t m_rectVao;
	static uint32_t m_rectVbo;
	static uint32_t m_rectEbo;

	static uint32_t m_partialRectVao;
	static uint32_t m_partialRectVbo;
	static uint32_t m_partialRectEbo;

	static glm::mat4 m_projection;

	static int m_windowWidth;
	static int m_windowHeight;
};
