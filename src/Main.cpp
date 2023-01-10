#include <fmt/core.h>
#include <fmt/color.h>
#include <GLFW/glfw3.h>

int main()
{
	fmt::print("pozdravljen svet\n");

	if (!glfwInit())
		fmt::print(fg(fmt::color::red), "Failed to init glfw!\n");

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Gravene", nullptr, nullptr);
	if (window == nullptr)
		fmt::print(fg(fmt::color::red), "Failed to create a glfw window!\n");

	glfwMakeContextCurrent(window);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}