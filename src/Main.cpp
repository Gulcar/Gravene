#include <fmt/core.h>
#include <fmt/color.h>

#define ASIO_SEPARATE_COMPILATION
#define _WIN32_WINNT 0x0601
#include <asio/impl/src.hpp>
#include <asio.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stb_image.h>

#include "Renderer.h"

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main()
{
	fmt::print("pozdravljen svet\n");

	Renderer::Init();

	while (!glfwWindowShouldClose(Renderer::GetWindow()))
	{
		glfwPollEvents();

		if (glfwGetKey(Renderer::GetWindow(), GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(Renderer::GetWindow(), true);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(Renderer::GetWindow());
	}

	glfwTerminate();
}