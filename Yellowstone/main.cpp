#include "app.hpp"

#include <iostream>
#include <cstdlib>
#include <stdexcept>

int main() {
	yellowstone::App app{};

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}