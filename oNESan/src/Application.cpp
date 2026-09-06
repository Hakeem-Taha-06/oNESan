#include "Application.h"

Application::Application(int width, int height): window(width, height), lastTime(std::chrono::steady_clock::now()){
	
}

Application::~Application() {

}

void Application::run() {
	emulator.loadCartridge("src/ROMs/nestest.nes");
	emulator.reset();
	while (!window.shouldExit()) {
		auto currentTime = std::chrono::steady_clock::now();
		double deltaTime = std::chrono::duration<double>(currentTime - lastTime).count();
		lastTime = currentTime;

		if (deltaTime > 0.5) deltaTime = 0.5;

		if (window.getEmulationMode() == EmulationMode::Automatic) {
			for(int i =0; i < static_cast<int>(deltaTime*NES_CPU_HZ*3); i++)
				emulator.clock();
		}
		else if (window.getEmulationMode() == EmulationMode::Debug) {
			emulator.clock();
		}

		window.startFrame();
		window.render(emulator);
		window.endFrame();

#ifdef DEBUG
		debugOutTime += deltaTime;
		if (debugOutTime > debugOutTimeLimit) {
			std::cout << "Delta Time: " << deltaTime << ", CPU Clocks: " << deltaTime * NES_CPU_HZ << '\n';
			debugOutTime = 0.0;
		}
#endif // DEBUG
	}
}