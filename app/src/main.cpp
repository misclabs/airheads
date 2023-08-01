#define SDL_MAIN_HANDLED

#include "app.h"

int main() {
	if (auto app = Airheads::App::CreateApplication()) {
		app->RunMainLoop();
	}

	return 0;
}
