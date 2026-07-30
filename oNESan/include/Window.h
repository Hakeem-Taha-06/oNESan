#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <iostream>

class Emulator;

class Window
{
public:
	Window(int width, int height);
	~Window();

	void init();
	void startFrame();
	void render(const Emulator& emulator);
	void endFrame();

	inline bool shouldExit() const { return glfwWindowShouldClose(m_window); }
private:
	GLFWwindow* m_window;

	int WIDTH = 800, HEIGHT = 600;
};

