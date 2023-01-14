#include <fmt/core.h>
#include <fmt/color.h>

#include <asio.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stb_image.h>

#include "Renderer.h"

int main()
{
	fmt::print("pozdravljen svet\n");

	Renderer::Init();

	uint32_t amogusImage = Renderer::LoadTexture("resources/amogus.png");
	uint32_t font = Renderer::LoadTexture("resources/bitmap_font.png");

	while (!glfwWindowShouldClose(Renderer::GetWindow()))
	{
		if (glfwGetKey(Renderer::GetWindow(), GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(Renderer::GetWindow(), true);

		Renderer::NewFrame();

		Renderer::Draw(amogusImage, { std::sinf(glfwGetTime()) * 5.0f, 0}, {10, 5}, glfwGetTime() * 180.0f, {std::sinf(glfwGetTime()), std::cosf(glfwGetTime()), std::tanf(glfwGetTime())});
		Renderer::DrawPartial(font, { 0,0 }, { 6.4f, 9.6f }, { 0, 96 }, { 64 * 2, 96 * 2 }, {1024, 576});
	}

	Renderer::Destroy();
}