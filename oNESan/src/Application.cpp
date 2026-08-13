#include "Application.h"

Application::Application(int width, int height): window(width, height) {
	
}

Application::~Application() {

}

void Application::run() {
	emulator.loadCartridge("src/ROMs/nestest.nes");
	emulator.reset();
	while (!window.shouldExit()) {
		window.startFrame();
		window.render(emulator);
		if (window.getEmulationMode() == EmulationMode::Automatic) {
			emulator.clock();
		}
		window.endFrame();
	}
}