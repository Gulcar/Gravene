#pragma once

#include <glm/vec2.hpp>
#include <string_view>

namespace UI
{

class Element
{
public:
	virtual void Update() = 0;
	virtual void Draw() = 0;
	inline void SetPos(glm::vec2 pos) { m_pos = pos; }
	inline void SetSize(glm::vec2 size) { m_size = size; }
	inline void SetPosAndSize(glm::vec2 pos, glm::vec2 size) { m_pos = pos; m_size = size; }

protected:
	glm::vec2 m_pos;
	glm::vec2 m_size;
};


class Button : public Element
{
public:
	Button();
	void Update() override;
	void Draw() override;
	inline void SetText(std::string_view text) { m_text = text; }
	inline bool Clicked() { return m_clicked; }

private:
	uint32_t m_texture;
	std::string m_text;
	bool m_isHovering = false;
	bool m_clicked = false;
};


class InputField : public Element
{
public:
	InputField();
	~InputField();
	void Update() override;
	void Draw() override;
	std::string& GetInput() { return m_text; }

private:
	static void GlfwCharCallback(struct GLFWwindow* window, unsigned int codepoint);

private:
	uint32_t m_texture;
	std::string m_text;
	bool m_isActive = false;
	bool m_isHovering = false;

	static InputField* s_currentInputField;
};

}
