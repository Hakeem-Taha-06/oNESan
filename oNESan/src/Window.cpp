#include "Window.h"
#include "Emulator.h"

Window::Window(int width, int height): WIDTH(width), HEIGHT(height), inputManager(){
	init();
}

Window::~Window() {

	// ImGui termination (all debug ui stuff can be moved to a separate ui class)
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}

void Window::init() {
	if (!glfwInit()) {
		std::cerr << "glfw not initialized";
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
	GLFWwindow* w = glfwCreateWindow(WIDTH, HEIGHT, "oNESan", nullptr, nullptr);
	if (w == nullptr) {
		std::cout << "Failed to create window";

		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	context = WindowContext{&inputManager, EmulationMode::Manual};

	glfwMakeContextCurrent(w);
	glfwSwapInterval(1);
	glfwSetWindowUserPointer(w, &context);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD";
		exit(EXIT_FAILURE);
	}

	std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "GL Version: " << glGetString(GL_VERSION) << std::endl;

	glViewport(0, 0, WIDTH, HEIGHT);

	glfwSetKeyCallback(w, Window::keyEventCallback);
	//setGLFWEventCallbacks(w);

	// Setup Dear ImGui context (all debug ui stuff can be moved to a separate ui class)
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Control
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	
	ImGui::StyleColorsDark();                                 // Configure Style Color
	// Setup scaling
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);                          // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	style.FontScaleDpi = main_scale;                          // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)
#if GLFW_VERSION_MAJOR >= 3 && GLFW_VERSION_MINOR >= 3
	io.ConfigDpiScaleFonts = true;                            // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
	io.ConfigDpiScaleViewports = true;                        // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.
#endif

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(w, true);                    // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init("#version 330");

	m_window = w;

}

void Window::render(Emulator& emulator) {
	
	processInput(emulator);

	//rendering
	glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// IMGUI (all debug ui stuff can be moved to a separate ui class)
	// Create a dockspace that covers the entire viewport
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport());

	renderControlWindow(emulator);
	renderWatchWindow(emulator);

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	renderMemoryWindow(emulator);
	ImGui::PopStyleVar(1);

	renderDisassemblyWindow(emulator);
	renderCHRROMWindow(emulator);
	renderOAMWindow(emulator);

	renderGameScreen(emulator);
	
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}

}

void Window::renderControlWindow(Emulator& emulator) {
	ImGui::Begin("Debug Window");

	ImGui::InputText("##RomPath", &testRomPath);
	ImGui::SameLine();
	if (ImGui::Button("Browse")) {
		const char* filterPatters[1] = { "*.nes" };
		const char* path = tinyfd_openFileDialog(
			"Choose .nes ROM file",
			"",
			1,
			filterPatters,
			".nes",
			0
		);
		if (path)
			testRomPath = path;
	}
	if (ImGui::Button("Load Test ROM")) {
		try {
			emulator.loadCartridge(testRomPath);
		}
		catch (std::exception e) {
			std::cerr << e.what() << std::endl;
		}
	}

	if (ImGui::Button("Clock Emulator")) {
		emulator.clock();
	}

	ImGui::InputInt("##testClocks", &testClocks);
	ImGui::SameLine();
	if(ImGui::Button("Clock # Times")) {
		
		for (int i = 0; i < testClocks; ++i) {
			emulator.clock();
		}
	}

	ImGui::InputScalar("##targetPC", ImGuiDataType_U16, &targetPC, nullptr, nullptr, "%04X", ImGuiInputTextFlags_CharsHexadecimal);
	ImGui::SameLine();
	if (ImGui::Button("Clock Until Address")) {
		for (int i = 0; i < MAX_CLOCKS; ++i) {
			emulator.clock();
			if (emulator.bus.cpu.pc >= targetPC) break;
		}
	}

	ImGui::End();
}

void Window::renderWatchWindow(Emulator& emulator) {
	ImGui::Begin("Registers Watch");

	if (ImGui::TreeNode("Cpu")) {
		ImGui::Text("Accumulator:     0x%02X", emulator.bus.cpu.acc);
		ImGui::Text("X Register:      0x%02X", emulator.bus.cpu.xreg);
		ImGui::Text("Y Register:      0x%02X", emulator.bus.cpu.yreg);
		ImGui::Text("Stack Pointer:   0x%04X", emulator.bus.cpu.stkp);
		ImGui::Text("Program Counter: 0x%04X", emulator.bus.cpu.pc);

		ImGui::Separator();

		ImGui::Text("Carry:             %i", emulator.bus.cpu.getFlag(nes6502::C));
		ImGui::Text("Zero:              %i", emulator.bus.cpu.getFlag(nes6502::Z));
		ImGui::Text("Interrupt Disable: %i", emulator.bus.cpu.getFlag(nes6502::I));
		ImGui::Text("Decimal Mode:      %i", emulator.bus.cpu.getFlag(nes6502::D));
		ImGui::Text("Break:             %i", emulator.bus.cpu.getFlag(nes6502::B));
		ImGui::Text("Overflow:          %i", emulator.bus.cpu.getFlag(nes6502::V));
		ImGui::Text("Negative:          %i", emulator.bus.cpu.getFlag(nes6502::N));

		ImGui::TreePop();
	}

	ImGui::Text("joypad input:     %s", std::bitset<8>(emulator.bus.joypad.input).to_string().c_str());
	ImGui::Text("joypad shift reg: %s", std::bitset<8>(emulator.bus.joypad.shiftReg).to_string().c_str());
	ImGui::Text("Total Cycles:      %i", emulator.bus.cpu.totalCycles);
	ImGui::End();
}

void Window::renderMemoryWindow(Emulator& emulator) {
	ImGui::Begin("Memory");

	// setup
	size_t ramSize = 64*1024;
	int numCols = 16;
	int numRows = (int)ramSize / numCols;
	ImGuiTableFlags flags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg;

	ImGui::Checkbox("Follow PC", &memoryFollowPC);

	if (memoryFollowPC) {
		// set target y scroll based on pc row
		int pcRow = emulator.bus.cpu.pc / numCols;
		ImGuiStyle& style = ImGui::GetStyle();
		float rowHeight = ImGui::GetTextLineHeight() + style.CellPadding.y * 2.0f;
		targetMemoryYScroll = pcRow*rowHeight;
	}

	// Memory address search
	bool wantSearch = false;
	ImGui::Text("Search Address");
	ImGui::InputScalar("##address", ImGuiDataType_U16, &searchAddress, nullptr, nullptr, "%04X", ImGuiInputTextFlags_CharsHexadecimal);
	ImGui::SameLine();
	if (ImGui::Button("GOTO") && !wantSearch) {
		wantSearch = true;
		memoryFollowPC = false;
		int searchRow = searchAddress / numCols;
		ImGuiStyle& style = ImGui::GetStyle();
		float rowHeight = ImGui::GetTextLineHeight() + style.CellPadding.y * 2.0f;
		targetMemoryYScroll = searchRow * rowHeight;

	}

	if (ImGui::BeginTable("Ram", numCols + 1, flags)) {

		// set fixed width for columns
		ImGui::TableSetupColumn("Offset", ImGuiTableColumnFlags_WidthFixed, 60.0f);
		for (int i = 0; i < numCols; i++) {
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 32.0f);
		}

		if (wantSearch) {
			ImGui::SetScrollY(targetMemoryYScroll);
			wantSearch = false;
		}

		if (memoryFollowPC) {
			ImGui::SetScrollY(targetMemoryYScroll);
		}

		ImGuiListClipper clipper;
		clipper.Begin(numRows);

		while (clipper.Step()) {
			for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%04X", row * numCols);

				for (int col = 0; col < numCols; ++col) {
					ImGui::TableSetColumnIndex(col + 1);

					uint16_t byteIdx = row * numCols + col;
					ImGui::PushID((int)byteIdx);
					ImGui::PushItemWidth(-FLT_MIN);

					uint8_t data = emulator.bus.cpuRead(byteIdx);

					bool isPcIdx = emulator.bus.cpu.pc == byteIdx;
					bool isStkIdx = emulator.bus.cpu.stkp + 0x100 == byteIdx;
					
					ImVec4 textColor{1.0f, 1.0f, 1.0f, 1.0f};

					if (isPcIdx) 
						textColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);

					if(isStkIdx)
						textColor = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);

					if (byteIdx < ramSize) {
						ImGui::TextColored(textColor, "%02X", data);
					}

					ImGui::PopItemWidth();
					ImGui::PopID();
				}
			}
		}

		ImGui::EndTable();
	}
	ImGui::End();
}


void Window::renderDisassemblyWindow(Emulator& emulator) {
	ImGui::Begin("Disassembly");

	ImGui::Checkbox("Follow PC", &disassemblerFollowPC);
	ImGui::SameLine();
	if (ImGui::Button("<")) {
		currentAddr--;
		disassemblerFollowPC = false;
	}
	ImGui::SameLine();
	if (ImGui::Button(">")) {
		currentAddr++;
		disassemblerFollowPC = false;
	}

	if (ImGui::InputScalar("##disassembleraddr", ImGuiDataType_U16, &currentAddr, 0, 0, "%04X", ImGuiInputTextFlags_CharsHexadecimal)) {
		disassemblerFollowPC = false;
	}

	if (disassemblerFollowPC) currentAddr = emulator.bus.cpu.pc;


	uint16_t startAddr = currentAddr;
	uint16_t endAddr = currentAddr + disassemblyDisplaySize;

	disassemblyLines = emulator.bus.cpu.disassemble(startAddr, endAddr);

	if (ImGui::BeginTable("##disassembly", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit)) {
		
		//ImGuiListClipper clipper;
		//clipper.Begin(disassemblyLines.size());
		//
		//while (clipper.Step()) {
		//	for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
		//
		//	}
		//}

		int i = 0;
		for (auto& [addr, instr] : disassemblyLines) {
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			if (addr == emulator.bus.cpu.pc) {
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", instr.c_str());
			}
			else {
				ImGui::Text("%s", instr.c_str());
			}
			++i;
		}

		ImGui::EndTable();
	}

	ImGui::End();
}

void Window::renderCHRROMWindow(Emulator& emulator) {
	ImGui::Begin("CHRROM");
	ImGui::InputInt("##selectedpalette", &selectedPalette);
	const float *scr1 = emulator.bus.ppu.getPatternTable(0, selectedPalette);
	const float *scr2 = emulator.bus.ppu.getPatternTable(1, selectedPalette);

	if(CHRROMScreenTexture1 == 0)
		glGenTextures(1, &CHRROMScreenTexture1);
	if(CHRROMScreenTexture2 == 0)
		glGenTextures(1, &CHRROMScreenTexture2);
		
	// screen 1
	glBindTexture(GL_TEXTURE_2D, CHRROMScreenTexture1);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_FLOAT, scr1); 

	// screen 2
	glBindTexture(GL_TEXTURE_2D, CHRROMScreenTexture2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_FLOAT, scr2);

	ImTextureID imScrn1Tex = (ImTextureID)(intptr_t)CHRROMScreenTexture1;
	ImTextureID imScrn2Tex = (ImTextureID)(intptr_t)CHRROMScreenTexture2;
	ImGui::GetWindowDrawList()->AddCallback(ImGui::GetPlatformIO().DrawCallback_SetSamplerNearest, nullptr);
	ImGui::Image(imScrn1Tex, ImVec2(128*2, 128*2));
	ImGui::SameLine();
	ImGui::Image(imScrn2Tex, ImVec2(128*2, 128*2));
	ImGui::GetWindowDrawList()->AddCallback(ImGui::GetPlatformIO().DrawCallback_SetSamplerLinear, nullptr);

	glBindTexture(GL_TEXTURE_2D, 0);
	ImGui::End();
}

void Window::renderGameScreen(Emulator& emulator) {
	ImGui::Begin("Screen");
	const float* screen = emulator.bus.ppu.getScreen();

	if (screenTexture == 0)
		glGenTextures(1, &screenTexture);

	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 240, 0, GL_RGB, GL_FLOAT, screen);

	ImTextureID imScrnTex = (ImTextureID)(intptr_t)screenTexture;
	ImGui::GetWindowDrawList()->AddCallback(ImGui::GetPlatformIO().DrawCallback_SetSamplerNearest, nullptr);
	ImGui::Image(imScrnTex, ImVec2(256*2, 240*2));
	ImGui::GetWindowDrawList()->AddCallback(ImGui::GetPlatformIO().DrawCallback_SetSamplerLinear, nullptr);

	ImGui::End();
}

void Window::renderOAMWindow(Emulator& emulator) {
	ImGui::Begin("OAM");

	if (ImGui::BeginTable("##oamlist", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit)) {
		for (int i = 0; i < 16; ++i) {
			ImGui::TableNextRow();
			
			ImGui::TableSetColumnIndex(0);
			uint8_t y      = emulator.bus.ppu.oamArray[i * 4 + 0];
			uint8_t id     = emulator.bus.ppu.oamArray[i * 4 + 1];
			uint8_t attrib = emulator.bus.ppu.oamArray[i * 4 + 2];
			uint8_t x      = emulator.bus.ppu.oamArray[i * 4 + 3];
			ImGui::Text("(%03d, %03d), ID: %03d, ATT: %03d", x, y, id, attrib);
		}
		ImGui::EndTable();
	}

	ImGui::End();
}

void Window::startFrame() {
	glfwPollEvents();

	// Start the Dear ImGui frame (all debug ui stuff can be moved to a separate ui class)
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Window::endFrame() {
	glfwSwapBuffers(m_window);
}

void Window::keyEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	auto context = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
	
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		context->emulationMode = EmulationMode::Automatic;
	}

	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		context->emulationMode = EmulationMode::Manual;
	}

	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		context->emulationMode = EmulationMode::Debug;
	}
	
	if (action == GLFW_PRESS) {
		context->input->setKeyState(key, true);
	}
	else if (action == GLFW_RELEASE) {
		context->input->setKeyState(key, false);
	}
}

void Window::processInput(Emulator& emulator) {
	if (inputManager.isKeyPressed(GLFW_KEY_W))
		emulator.bus.joypad.pressButton(Controller::Button::Up);
	else
		emulator.bus.joypad.releaseButton(Controller::Button::Up);

	if(inputManager.isKeyPressed(GLFW_KEY_S))
		emulator.bus.joypad.pressButton(Controller::Button::Down);
	else
		emulator.bus.joypad.releaseButton(Controller::Button::Down);

	if (inputManager.isKeyPressed(GLFW_KEY_A))
		emulator.bus.joypad.pressButton(Controller::Button::Left);
	else
		emulator.bus.joypad.releaseButton(Controller::Button::Left);

	if (inputManager.isKeyPressed(GLFW_KEY_D))
		emulator.bus.joypad.pressButton(Controller::Button::Right);
	else
		emulator.bus.joypad.releaseButton(Controller::Button::Right);

	if (inputManager.isKeyPressed(GLFW_KEY_G))
		emulator.bus.joypad.pressButton(Controller::Button::B);
	else
		emulator.bus.joypad.releaseButton(Controller::Button::B);

	if (inputManager.isKeyPressed(GLFW_KEY_H))
		emulator.bus.joypad.pressButton(Controller::Button::A);
	else
		emulator.bus.joypad.releaseButton(Controller::Button::A);

	if (inputManager.isKeyPressed(GLFW_KEY_ENTER))
		emulator.bus.joypad.pressButton(Controller::Button::Start);
	else
		emulator.bus.joypad.releaseButton(Controller::Button::Start);

	if (inputManager.isKeyPressed(GLFW_KEY_J))
		emulator.bus.joypad.pressButton(Controller::Button::Select);
	else
		emulator.bus.joypad.releaseButton(Controller::Button::Select);
}