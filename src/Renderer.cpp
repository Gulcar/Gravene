#include "Renderer.h"

#include <fmt/core.h>
#include <fmt/color.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

static const char* vertexShaderSource = R"(
#version 430 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);
	TexCoords = aTexCoords;
}
)";

static const char* fragmentShaderSource = R"(
#version 430 core

in vec2 TexCoords;
uniform vec4 colorTint;
out vec4 FragColor;

uniform sampler2D texture1;

void main()
{
	FragColor = texture(texture1, TexCoords) * colorTint;
}
)";

static void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Renderer::Init()
{
	if (!glfwInit())
		fmt::print(fg(fmt::color::red), "Failed to init glfw!\n");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_window = glfwCreateWindow(1280, 720, "Gravene", nullptr, nullptr);
	if (m_window == nullptr)
		fmt::print(fg(fmt::color::red), "Failed to create a glfw window!\n");

	glfwMakeContextCurrent(m_window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);
	glViewport(0, 0, 1280, 720);

	m_defaultShader = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
}

uint32_t Renderer::LoadImage(std::string_view filename)
{
	return -1;
}

void Renderer::Draw(uint32_t imageId, glm::vec2 pos, glm::vec2 size, float rotation, Rgb color)
{

}

GLFWwindow* Renderer::GetWindow()
{
	return m_window;
}

uint32_t Renderer::CreateShaderProgram(const char* vertexSource, const char* fragmentSource)
{
	uint32_t vertexShader = CreateShader(vertexSource, GL_VERTEX_SHADER);
	uint32_t fragmentShader = CreateShader(fragmentSource, GL_FRAGMENT_SHADER);

	uint32_t shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	int success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		int logLength;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
		
		char* log = (char*)alloca(logLength * sizeof(char));
		glGetProgramInfoLog(shaderProgram, logLength, nullptr, log);

		fmt::print(fg(fmt::color::red), "Failed to compile shader program!\n{}\n", log);
	}

	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

uint32_t Renderer::CreateShader(const char* source, uint32_t type)
{
	uint32_t shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		int logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

		char* log = (char*)alloca(logLength * sizeof(char));
		glGetShaderInfoLog(shader, logLength, nullptr, log);

		fmt::print(fg(fmt::color::red), "Failed to compile shader!\n{}\n", log);
	}

	return shader;
}

void Renderer::CheckGlErrors()
{
	while (GLenum err = glGetError())
	{
		switch (err)
		{
		case GL_INVALID_ENUM: fmt::print(fg(fmt::color::red), "OpenGL Error GL_INVALID_ENUM\n"); break;
		case GL_INVALID_VALUE: fmt::print(fg(fmt::color::red), "OpenGL Error GL_INVALID_VALUE\n"); break;
		case GL_INVALID_OPERATION: fmt::print(fg(fmt::color::red), "OpenGL Error GL_INVALID_OPERATION\n"); break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: fmt::print(fg(fmt::color::red), "OpenGL Error GL_INVALID_FRAMEBUFFER_OPERATION\n"); break;
		case GL_OUT_OF_MEMORY: fmt::print(fg(fmt::color::red), "OpenGL Error GL_OUT_OF_MEMORY\n"); break;
		default: fmt::print(fg(fmt::color::red), "Weird OpenGL Error!\n");
		}
	}

}

void Renderer::Destroy()
{
	glDeleteShader(m_defaultShader);
}

std::unordered_map<std::string, uint32_t> Renderer::m_loadedImages;
GLFWwindow* Renderer::m_window;
uint32_t Renderer::m_defaultShader;