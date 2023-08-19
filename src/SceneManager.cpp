#include "SceneManager.h"

#include "Renderer.h"
#include "GLFW/glfw3.h"
#include <fmt/core.h>
#include <fmt/color.h>

void SceneManager::SwitchToScene(const std::string& sceneName)
{
    fmt::print("switching to scene: {}\n", sceneName);

    s_switchTo = s_scenes[sceneName].get();

#ifndef NDEBUG
    if (s_switchTo == nullptr)
    {
        fmt::print(fg(fmt::color::red), "The scene to switch to does not exist!! ({})\n", sceneName);
        throw std::runtime_error("The scene to switch to does not exist!! " + sceneName);
    }
#endif
}

void SceneManager::Update()
{
    static float prevTime = 0.0;
    float time = glfwGetTime();
    float deltaTime = time - prevTime;
    prevTime = time;

    if (s_switchTo)
    {
        if (s_currentScene)
            s_currentScene->End();

        s_currentScene = s_switchTo;
        s_switchTo = nullptr;

        s_currentScene->Start();
    }

    if (s_currentScene)
    {
        s_currentScene->Update(deltaTime);
        s_currentScene->Draw(deltaTime);
    }
}

void SceneManager::Exit()
{
    glfwSetWindowShouldClose(Renderer::GetWindow(), true);
}

std::unordered_map<std::string, std::unique_ptr<Scene>> SceneManager::s_scenes;
Scene* SceneManager::s_currentScene = nullptr;
