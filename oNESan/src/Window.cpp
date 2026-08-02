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
	GLFWwindow* w = glfwCreateWindow(WIDTH, HEIGHT, "Hello, Window!", nullptr, nullptr);
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

	glfwSetKeyCallback(m_window, Window::keyEventCallback);
}

void Window::render(Emulator& emulator) {
	
	processInput();

	//rendering
	glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// IMGUI (all debug ui stuff can be moved to a separate ui class)
	// Create a dockspace that covers the entire viewport
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport());

	renderControlWindow(emulator);
	renderCPUWindow(emulator);

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	renderMemoryWindow(emulator);
	ImGui::PopStyleVar(1);

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
	ImGui::Text("Value to be Put in Accumulator: ");

	int value = emulator.testOperand;
	value = std::clamp(value, 0, 255);
	if (ImGui::InputInt("Value", &value)) {
		emulator.testOperand = value;
	}
	if (ImGui::Button("Reset Test Program")) {
		emulator.writeTestProgram2();
	}
	if (ImGui::Button("Clock Emulator")) {
		emulator.clock();
	}
	if (ImGui::Button("Push to Stack")) {
		emulator.bus.cpu.push(0xBB);
	}
	if (ImGui::Button("Pop From Stack")) {
		emulator.bus.cpu.pop();
	}

	ImGui::End();
}

void Window::renderCPUWindow(Emulator& emulator) {
	ImGui::Begin("CPU");

	if (ImGui::TreeNode("Registers")) {
		ImGui::Text("Accumulator:     0x%02X", emulator.bus.cpu.acc);
		ImGui::Text("X Register:      0x%02X", emulator.bus.cpu.xreg);
		ImGui::Text("Y Register:      0x%02X", emulator.bus.cpu.yreg);
		ImGui::Text("Stack Pointer:   0x%04X", emulator.bus.cpu.stkp);
		ImGui::Text("Program Counter: 0x%04X", emulator.bus.cpu.pc);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Status")) {
		ImGui::Text("Carry:             %i", emulator.bus.cpu.getFlag(nes6502::C));
		ImGui::Text("Zero:              %i", emulator.bus.cpu.getFlag(nes6502::Z));
		ImGui::Text("Interrupt Disable: %i", emulator.bus.cpu.getFlag(nes6502::I));
		ImGui::Text("Decimal Mode:      %i", emulator.bus.cpu.getFlag(nes6502::D));
		ImGui::Text("Break:             %i", emulator.bus.cpu.getFlag(nes6502::B));
		ImGui::Text("Overflow:          %i", emulator.bus.cpu.getFlag(nes6502::V));
		ImGui::Text("Negative:          %i", emulator.bus.cpu.getFlag(nes6502::N));

		ImGui::TreePop();
	}
	ImGui::End();
}

void Window::renderMemoryWindow(Emulator& emulator) {
	ImGui::Begin("Memory");

	// setup
	size_t ramSize = emulator.bus.ram.size();
	int numCols = 8;
	int numRows = (int)ramSize / numCols;
	ImGuiTableFlags flags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg;

	ImGui::Checkbox("Follow PC", &followPC);

	if (followPC) {
		// set target y scroll based on pc row
		int pcRow = emulator.bus.cpu.pc / numCols;
		ImGuiStyle& style = ImGui::GetStyle();
		float rowHeight = ImGui::GetTextLineHeight() + style.CellPadding.y * 2.0f + style.FramePadding.y * 2;
		targetMemoryYScroll = pcRow*rowHeight;
	}

	// Memory address search
	bool wantSearch = false;
	ImGui::Text("Search Address");
	ImGui::InputScalar("##address", ImGuiDataType_U16, &searchAddress, nullptr, nullptr, "%04X", ImGuiInputTextFlags_CharsHexadecimal);
	ImGui::SameLine();
	if (ImGui::Button("GOTO") && !wantSearch) {
		wantSearch = true;
		followPC = false;
		int searchRow = searchAddress / numCols;
		ImGuiStyle& style = ImGui::GetStyle();
		float rowHeight = ImGui::GetTextLineHeight() + style.CellPadding.y * 2.0f + style.FramePadding.y * 2;
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

		if (followPC) {
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

					size_t byteIdx = row * numCols + col;
					ImGui::PushID((int)byteIdx);
					ImGui::PushItemWidth(-FLT_MIN);

					uint8_t& data = emulator.bus.ram[byteIdx];

					bool isPcIdx = emulator.bus.cpu.pc == byteIdx;
					bool isStkIdx = emulator.bus.cpu.stkp + 0x100 == byteIdx;
					
					// color the box where the program counter is
					if (isPcIdx) 
						ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.6f, 0.0f, 0.3f, 1.0f));

					if(isStkIdx)
						ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.3f, 0.0f, 0.6f, 1.0f));

					if (byteIdx < ramSize) {
						ImGui::InputScalar("##data", ImGuiDataType_U8, &data, nullptr, nullptr, "%02X", ImGuiInputTextFlags_CharsHexadecimal);
					}

					// only pop the style color if we had pushed it above
					if (isPcIdx) 
						ImGui::PopStyleColor(1);

					if (isStkIdx)
						ImGui::PopStyleColor(1);

					ImGui::PopItemWidth();
					ImGui::PopID();
				}
			}
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
		if (context->emulationMode == EmulationMode::Manual)
			context->emulationMode = EmulationMode::Automatic;
		else if (context->emulationMode == EmulationMode::Automatic)
			context->emulationMode = EmulationMode::Manual;
	}
	
	if (action == GLFW_PRESS) {
		context->input->setKeyState(key, true);
	}
	else if (action == GLFW_RELEASE) {
		context->input->setKeyState(key, false);
	}
}

void Window::processInput() {
	
}