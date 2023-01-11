#include <fmt/core.h>
#include <fmt/color.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main()
{
	fmt::print("pozdravljen svet\n");

	if (!glfwInit())
		fmt::print(fg(fmt::color::red), "Failed to init glfw!\n");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Gravene", nullptr, nullptr);
	if (window == nullptr)
		fmt::print(fg(fmt::color::red), "Failed to create a glfw window!\n");

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	glViewport(0, 0, 1280, 720);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(window, true);

		glClearColor(1.0f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
	}

	glfwTerminate();
}