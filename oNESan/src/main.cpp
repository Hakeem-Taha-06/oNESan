#include <cstdint>
#include <iostream>

#include "Application.h"

int main() {

	Application* a = new Application{1600, 800};

	a->run();

	delete a;

	return 0;
}