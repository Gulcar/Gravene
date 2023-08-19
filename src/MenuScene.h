#pragma once

#include "SceneManager.h"
#include "UI.h"

class MenuScene : public Scene
{
public:
    void Start() override;
    void Update(float deltaTime) override;
    void Draw(float deltaTime) override;
    void End() override;

private:
    uint32_t m_starsTexture;
    UI::Button m_playBtn;
    UI::InputField m_playerNameInput;
    UI::InputField m_serverIpInput;
};
