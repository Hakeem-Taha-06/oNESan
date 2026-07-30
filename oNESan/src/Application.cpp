#include "Application.h"

Application::Application(int width, int height): window(width, height) {
	
}

Application::~Application() {

}

void Application::run() {
	emulator.runTestProgram();
	while (!window.shouldExit()) {
		window.startFrame();
		window.render(emulator);
		window.endFrame();
	}
}