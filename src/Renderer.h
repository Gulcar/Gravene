#pragma once

#include <cstdint>
#include <string>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <unordered_map>

class Renderer
{
public:

	static void Init();

	static uint32_t LoadTexture(const std::string& filename);
	static void Draw(uint32_t textureId, glm::vec2 pos, glm::vec2 size, float rotation = 0.0f, glm::vec3 color = {1.0f, 1.0f, 1.0f});
	static void DrawPartial(uint32_t textureId, glm::vec2 pos, glm::vec2 size,
		glm::vec2 pixelPos, glm::vec2 pixelSize, glm::vec2 textureSize, float rotation = 0.0f, glm::vec3 color = {1.0f, 1.0f, 1.0f});

	static void NewFrame();

	inline static struct GLFWwindow* GetWindow() { return m_window; }
	inline static int GetWindowWidth() { return m_windowWidth; }
	inline static int GetWindowHeight() { return m_windowHeight; }

	static void CheckGlErrors();

	static float GetRightEdgeWorldPos();

	inline static void SetCameraPos(const glm::vec2& pos) { m_cameraPos = pos; CreateViewMat(); }
	inline static glm::vec2 GetCameraPos() { return m_cameraPos; }

	static void Destroy();

private:

	static uint32_t CreateShaderProgram(const char* vertexSource, const char* fragmentSource);
	static uint32_t CreateShader(const char* source, uint32_t type);

	static void CreateRectBuffers();
	static void CreatePartialRectBuffers();

	static void CreateProjectionMat();
	static void CreateViewMat();

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
	static glm::mat4 m_view;

	static int m_windowWidth;
	static int m_windowHeight;

	static glm::vec2 m_cameraPos;
};
