#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <iostream>
#include <algorithm>

#include "InputManager.h"

class Emulator;

enum class EmulationMode {
	Automatic,
	Manual
};

struct WindowContext {
	InputManager* input;
	EmulationMode emulationMode = EmulationMode::Manual;
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
	inline EmulationMode getEmulationMode() const { 
		auto context = static_cast<WindowContext*>(glfwGetWindowUserPointer(m_window));
		return context->emulationMode; 
	}
private:
	GLFWwindow* m_window;
	InputManager inputManager;
	WindowContext context;

	int WIDTH = 800, HEIGHT = 600;

	bool followPC = false;
	uint16_t searchAddress = 0x0000;
	float targetMemoryYScroll = 0.0f;

	uint8_t testOperand = 0x00;

	void renderControlWindow(Emulator& emulator);
	void renderCPUWindow(Emulator& emulator);
	void renderMemoryWindow(Emulator& emulator);

	static void keyEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void processInput();
};

