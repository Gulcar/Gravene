#include <fmt/core.h>
#include <fmt/color.h>

#include <asio.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stb_image.h>

#include "Renderer.h"
#include "Text.h"
#include "Input.h"

int main()
{
	fmt::print("pozdravljen svet\n");

	Renderer::Init();
	Text::Init("resources/bitmap_font.png", { 1024, 576 }, { 64, 96 });
	Input::Init();

	uint32_t amogusImage = Renderer::LoadTexture("resources/amogus.png");

	while (!glfwWindowShouldClose(Renderer::GetWindow()))
	{
		static float prevTime = 0.0;
		float time = glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		if (glfwGetKey(Renderer::GetWindow(), GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(Renderer::GetWindow(), true);

		Renderer::NewFrame();

		Renderer::Draw(amogusImage, { std::sinf(glfwGetTime()) * 5.0f, 0}, {10, 5}, glfwGetTime() * 180.0f, {std::sinf(glfwGetTime()), std::cosf(glfwGetTime()), std::tanf(glfwGetTime())});

		Text::Write("Pozdravljen Svet!!", { 0.0f, 0.0f }, 1.0f, true);

		static float updateFpsCounter = 0.15f;
		static std::string fpsString = fmt::format("{}fps", (int)(1.0f / deltaTime));
		updateFpsCounter -= deltaTime;
		if (updateFpsCounter < 0.0f)
		{
			fpsString = fmt::format("{}fps", (int)(1.0f / deltaTime));
			updateFpsCounter = 0.15f;
		}
		Text::Write(fpsString, { -Renderer::GetRightEdgeWorldPos() + 0.5f, 9.4f}, 0.8f);
	}

	Renderer::Destroy();
}