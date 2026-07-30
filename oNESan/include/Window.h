#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <iostream>

class Window
{
public:
	Window(int width, int height);
	~Window();

	void init();
	void render();
private:
	GLFWwindow* m_window;

	int WIDTH = 800, HEIGHT = 600;
};

