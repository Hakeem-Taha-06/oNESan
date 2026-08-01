#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <iostream>
#include <algorithm>

class Emulator;

enum class EmulationMode {
	Automatic,
	Manual
};

class Window
{
public:
	Window(int width, int height);
	~Window();

	void init();
	void startFrame();
	void render(Emulator& emulator);
	void endFrame();

	inline bool shouldExit() const { return glfwWindowShouldClose(m_window); }
	inline EmulationMode getEmulationMode() const { return emulationMode; }
private:
	GLFWwindow* m_window;

	int WIDTH = 800, HEIGHT = 600;
	EmulationMode emulationMode = EmulationMode::Automatic;

	bool followPC = false;
	float targetMemoryYScroll = 0.0f;

	uint8_t testOperand = 0x00;

	void renderControlWindow(Emulator& emulator);
	void renderCPUWindow(Emulator& emulator);
	void renderMemoryWindow(Emulator& emulator);
};

