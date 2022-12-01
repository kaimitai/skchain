#include <SDL.h>
#include "./skc_constants/Constants_application.h"
#include "./common/klib/klib_file.h"
#include "./common/klib/User_input.h"
#include "./skc_windows/SKC_Main_window.h"
#include "./SKC_Config.h"
#include <iostream>
#include <vector>

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

using byte = unsigned char;

int main(int argc, char* args[]) try {
	SDL_Window* l_window{ nullptr };
	SDL_Renderer* l_rnd{ nullptr };
	bool l_exit{ false };

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		throw std::runtime_error(SDL_GetError());
	else {
		// Event handler
		SDL_Event e;

		l_window = SDL_CreateWindow(skc::c::APP_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, skc::c::APP_W, skc::c::APP_H, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
		if (l_window == nullptr)
			throw std::runtime_error(SDL_GetError());
		else {
			l_rnd = SDL_CreateRenderer(l_window, -1, SDL_RENDERER_ACCELERATED);

			if (l_rnd == nullptr)
				throw std::runtime_error(SDL_GetError());
			else {
				//Initialize renderer color
				SDL_SetRenderDrawColor(l_rnd, 0x00, 0x00, 0x00, 0x00);
			}
			/*
			// Setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			// Setup Dear ImGui style
			ImGui::StyleColorsDark();
			//ImGui::StyleColorsLight();

			// Setup Platform/Renderer backends
			ImGui_ImplSDL2_InitForSDLRenderer(l_window, l_rnd);
			ImGui_ImplSDLRenderer_Init(l_rnd);
			ImGui::GetIO().IniFilename = c::SP_IMGUI_INI_FILENAME;

			// main window object to handle all logic and drawing
			SP_Config config(argc > 1 ? args[1] : std::string());
			*/
			skc::SKC_Config l_config(klib::file::read_file_as_bytes("Solomon's Key (U) [!].nes"));
			skc::SKC_Main_window main_window(l_rnd, l_config);
			//main_window.set_application_icon(l_window);

			// input handler
			klib::User_input input;
			int mouse_wheel_y{ 0 };
			bool mw_used{ false };

			uint32_t last_logic_time = SDL_GetTicks() - 1;
			uint32_t last_draw_time = SDL_GetTicks() - 17;

			uint32_t delta = 1;
			uint32_t deltaDraw = 17;

			int l_w{ skc::c::APP_W }, l_h{ skc::c::APP_H };

			while (!l_exit) {

				uint32_t tick_time = SDL_GetTicks();	// current time

				deltaDraw = tick_time - last_draw_time;
				delta = tick_time - last_logic_time;
				int32_t mw_y{ 0 };

				mw_used = false;
				SDL_PumpEvents();

				if (SDL_PollEvent(&e) != 0) {
					//ImGui_ImplSDL2_ProcessEvent(&e);

					if (e.type == SDL_QUIT)
						l_exit = true;
					else if (e.type == SDL_MOUSEWHEEL) {
						mw_used = true;
						mouse_wheel_y = e.wheel.y;
					}
				}

				if (delta != 0) {
					uint32_t realDelta = std::min(delta, 5u);
					SDL_GetWindowSize(l_window, &l_w, &l_h);

					input.move(realDelta, mw_used ? mouse_wheel_y : 0);
					//main_window.move(realDelta, input, config, l_w, l_h);
					main_window.move(realDelta, input);

					last_logic_time = tick_time;
				}

				if (deltaDraw >= 25) { // capped frame rate of ~40 is ok
					//main_window.draw(l_rnd, input, config, l_w, l_h);
					main_window.draw(l_rnd, l_config);
					last_draw_time = SDL_GetTicks();

					//Update screen
					SDL_RenderPresent(l_rnd);
				}

				SDL_Delay(1);

			}
		}


	}

	// Destroy window
	SDL_DestroyWindow(l_window);

	// Quit SDL subsystems
	SDL_Quit();

	return 0;
}
catch (const std::exception& ex) {
	klib::file::append_string_to_file("Runtime error. Exception was:\n" + std::string(ex.what()) + "\n", skc::c::FILENAME_ERROR_LOG);
	return 1;
}
catch (...) {
	klib::file::append_string_to_file("Unknown runtime error occurred.\n", skc::c::FILENAME_ERROR_LOG);
	return 1;
}
