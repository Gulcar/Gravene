#pragma once

#include <string>
#include <memory>
#include <unordered_map>

class Scene
{
public:
	virtual void Start() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Draw(float deltaTime) = 0;
	virtual void End() = 0;
};

class SceneManager
{
public:
	static void SwitchToScene(const std::string& sceneName);

	static void Update();

	static void Exit();

	template<class SceneClass>
	static void AssignScene(const std::string& sceneName)
	{
		s_scenes.insert({ sceneName, std::make_unique<SceneClass>() });
	}

	inline static Scene* GetScene(const std::string& sceneName) { return s_scenes[sceneName].get(); }

private:
	static std::unordered_map<std::string, std::unique_ptr<Scene>> s_scenes;
	static Scene* s_currentScene;
	inline static Scene* s_switchTo = nullptr;
};
