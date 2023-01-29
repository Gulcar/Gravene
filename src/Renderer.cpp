#include "Renderer.h"

#include <fmt/core.h>
#include <fmt/color.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_projection.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

void Renderer::Init()
{
	fmt::print("Renderer::Init\n");

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

	glfwSetFramebufferSizeCallback(m_window, Renderer::GlfwFramebufferSizeCallback);
	GlfwFramebufferSizeCallback(m_window, 1280, 720);

	CreateViewMat();

	glfwSetErrorCallback(Renderer::GlfwErrorCallback);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_defaultShader = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);

	CreateRectBuffers();
	CreatePartialRectBuffers();

	stbi_set_flip_vertically_on_load(true);

	glfwSwapInterval(1);

	fmt::print("OpenGL Version: {}\n", (char*)glGetString(GL_VERSION));
}

uint32_t Renderer::LoadTexture(const std::string& filename)
{
	if (m_loadedTextures.find(filename) != m_loadedTextures.end())
	{
		fmt::print("Returned cached texture: {}\n", filename);
		return m_loadedTextures[filename];
	}

	int width, height, numOfChannles, numOfChannels;
	uint8_t* data = stbi_load(filename.c_str(), &width, &height, &numOfChannels, 0);
	if (!data) data = stbi_load(("../../../" + filename).c_str(), &width, &height, &numOfChannels, 0);
	if (!data) data = stbi_load(("../" + filename).c_str(), & width, & height, & numOfChannels, 0);

	uint32_t texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (data)
	{
		GLenum format = 0;
		switch (numOfChannels)
		{
		case 1: format = GL_RED; break;
		case 3: format = GL_RGB; break;
		case 4: format = GL_RGBA; break;
		default: fmt::print(fg(fmt::color::red), "Failed to get image format: {}\n", filename);
		}

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		m_loadedTextures.insert({ filename, texture });
		fmt::print("Loaded texture: {}\n", filename);
	}
	else
	{
		fmt::print(fg(fmt::color::red), "Failed to load texture: {}\n", filename);
	}

	return texture;
}

void Renderer::Draw(uint32_t textureId, glm::vec2 pos, glm::vec2 size, float rotation, glm::vec3 color)
{
	glm::mat4 model(1.0f);
	model = glm::translate(model, { pos.x, pos.y, 0.0f });
	model = glm::rotate(model, glm::radians(rotation), { 0.0f, 0.0f, 1.0f });
	model = glm::scale(model, { size.x, size.y, 1.0f });

	glUseProgram(m_defaultShader);
	glUniformMatrix4fv(glGetUniformLocation(m_defaultShader, "model"), 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(m_defaultShader, "view"), 1, GL_FALSE, &m_view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(m_defaultShader, "projection"), 1, GL_FALSE, &m_projection[0][0]);
	glUniform4f(glGetUniformLocation(m_defaultShader, "colorTint"), color.r, color.g, color.b, 1.0f);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glBindVertexArray(m_rectVao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::DrawPartial(uint32_t textureId, glm::vec2 pos, glm::vec2 size,
	glm::vec2 pixelPos, glm::vec2 pixelSize, glm::vec2 textureSize, float rotation, glm::vec3 color)
{
	glm::mat4 model(1.0f);
	model = glm::translate(model, { pos.x, pos.y, 0.0f });
	model = glm::rotate(model, glm::radians(rotation), { 0.0f, 0.0f, 1.0f });
	model = glm::scale(model, { size.x, size.y, 1.0f });

	glUseProgram(m_defaultShader);
	glUniformMatrix4fv(glGetUniformLocation(m_defaultShader, "model"), 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(m_defaultShader, "view"), 1, GL_FALSE, &m_view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(m_defaultShader, "projection"), 1, GL_FALSE, &m_projection[0][0]);
	glUniform4f(glGetUniformLocation(m_defaultShader, "colorTint"), color.r, color.g, color.b, 1.0f);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glm::vec2 posFraction = pixelPos / textureSize;
	glm::vec2 sizeFraction = pixelSize / textureSize;
	float vertices[] = {
		// pos			// tex coords
		-0.5f, -0.5f,	posFraction.x,						1.0f - posFraction.y - sizeFraction.y,
		 0.5f, -0.5f,	sizeFraction.x + posFraction.x,		1.0f - posFraction.y - sizeFraction.y,
		 0.5f,  0.5f,	sizeFraction.x + posFraction.x,		1.0f - posFraction.y,
		-0.5f,  0.5f,	posFraction.x,						1.0f - posFraction.y
	};

	glBindBuffer(GL_ARRAY_BUFFER, m_partialRectVbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(m_partialRectVao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::NewFrame()
{
	glfwSwapBuffers(m_window);
	glfwPollEvents();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
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

float Renderer::GetRightEdgeWorldPos()
{
	return 10.0f * m_windowWidth / m_windowHeight;
}

void Renderer::Destroy()
{
	fmt::print("Renderer::Destroy\n");

	CheckGlErrors();

	glDeleteShader(m_defaultShader);

	glDeleteVertexArrays(1, &m_rectVao);
	glDeleteBuffers(1, &m_rectVbo);
	glDeleteBuffers(1, &m_rectEbo);

	for (auto& [texName, texId] : m_loadedTextures)
		glDeleteTextures(1, &texId);

	glfwTerminate();
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

	fmt::print("Created shader program\n");

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

void Renderer::CreateRectBuffers()
{
	float vertices[] = {
		// pos			// tex coords
		-0.5f, -0.5f,	0.0f, 0.0f,
		 0.5f, -0.5f,	1.0f, 0.0f,
		 0.5f,  0.5f,	1.0f, 1.0f,
		-0.5f,  0.5f,	0.0f, 1.0f
	};

	uint32_t indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	glGenVertexArrays(1, &m_rectVao);
	glBindVertexArray(m_rectVao);

	glGenBuffers(1, &m_rectVbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_rectVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &m_rectEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rectEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	fmt::print("Created rect buffers\n");
}

void Renderer::CreatePartialRectBuffers()
{
	float vertices[] = {
		// pos			// tex coords
		-0.5f, -0.5f,	0.0f, 0.0f,
		 0.5f, -0.5f,	1.0f, 0.0f,
		 0.5f,  0.5f,	1.0f, 1.0f,
		-0.5f,  0.5f,	0.0f, 1.0f
	};

	uint32_t indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	glGenVertexArrays(1, &m_partialRectVao);
	glBindVertexArray(m_partialRectVao);

	glGenBuffers(1, &m_partialRectVbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_partialRectVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &m_partialRectEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_partialRectEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	fmt::print("Created partial rect buffers\n");
}

void Renderer::CreateProjectionMat()
{
	float ratio = (float)m_windowWidth / (float)m_windowHeight;

	m_projection = glm::ortho(-10.0f * ratio, 10.0f * ratio, -10.0f, 10.0f);
}

void Renderer::CreateViewMat()
{
	m_view = glm::translate(glm::mat4(1.0f), { -m_cameraPos.x, -m_cameraPos.y, 0.0f });
}

void Renderer::GlfwFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	m_windowWidth = width;
	m_windowHeight = height;
	Renderer::CreateProjectionMat();
}

void Renderer::GlfwErrorCallback(int errorCode, const char* desc)
{
	fmt::print(fg(fmt::color::red), "Glfw Error {}: {}\n", errorCode, desc);
}

std::unordered_map<std::string, uint32_t> Renderer::m_loadedTextures;
GLFWwindow* Renderer::m_window;

uint32_t Renderer::m_defaultShader;

uint32_t Renderer::m_rectVao;
uint32_t Renderer::m_rectVbo;
uint32_t Renderer::m_rectEbo;

uint32_t Renderer::m_partialRectVao;
uint32_t Renderer::m_partialRectVbo;
uint32_t Renderer::m_partialRectEbo;

glm::mat4 Renderer::m_projection;
glm::mat4 Renderer::m_view;

int Renderer::m_windowWidth;
int Renderer::m_windowHeight;

glm::vec2 Renderer::m_cameraPos(0.0f, 0.0f);
