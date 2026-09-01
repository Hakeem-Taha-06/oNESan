#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_stdlib.h>

#include "tinyfiledialogs.h"

#include <iostream>
#include <map>
#include <algorithm>
#include <bitset>

#include "InputManager.h"

class Emulator;

enum class EmulationMode {
	Automatic,
	Manual, 
	Debug
};

struct WindowContext {
	InputManager* input = nullptr;
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

	const int MAX_CLOCKS = 100'000'000;

	std::string testRomPath = "src/ROMs/nestest.nes";

	// Memory Viewer
	bool memoryFollowPC = false;
	int testClocks = 0;
	uint16_t targetPC = 0x0000;
	uint16_t searchAddress = 0x0000;
	float targetMemoryYScroll = 0.0f;

	// Disassembly Viewer
	bool disassemblerFollowPC = true;
	uint16_t currentAddr = 0x0000;
	std::map<uint16_t, std::string> disassemblyLines;
	int disassemblyDisplaySize = 20;

	// CHRROM Display
	unsigned int CHRROMScreenTexture1 = 0;
	unsigned int CHRROMScreenTexture2 = 0;
	int selectedPalette = 0;

	// Game Screen
	unsigned int screenTexture = 0;

	void renderControlWindow(Emulator& emulator);
	void renderWatchWindow(Emulator& emulator);
	void renderMemoryWindow(Emulator& emulator);
	void renderDisassemblyWindow(Emulator& emulator);
	void renderCHRROMWindow(Emulator& emulator);
	void renderGameScreen(Emulator& emulator);
	void renderOAMWindow(Emulator& emulator);

	static void keyEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void processInput(Emulator& emulator);
};

