#include <SDL.h>
#include "./common/klib/klib_file.h"
#include "./skc_windows/SKC_Main_window.h"
#include <iostream>
#include <vector>

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

using byte = unsigned char;
constexpr char ERROR_LOG_FILE[]{ "skc_errors.log" };

int main(int argc, char* args[]) try {
	SDL_Window* l_window{ nullptr };
	SDL_Renderer* l_rnd{ nullptr };
	bool l_exit{ false };

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		throw std::runtime_error(SDL_GetError());
	else {
		// Event handler
		SDL_Event e;

		l_window = SDL_CreateWindow("Solomon's Keychain", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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
			auto l_bytes = klib::file::read_file_as_bytes("Solomon's Key (U) [!].nes");
			skc::SKC_Main_window main_window(l_rnd, l_bytes);
			//main_window.set_application_icon(l_window);

			// input handler
			//klib::User_input input;
			int mouse_wheel_y{ 0 };
			bool mw_used{ false };

			uint32_t last_logic_time = SDL_GetTicks() - 1;
			uint32_t last_draw_time = SDL_GetTicks() - 17;

			uint32_t delta = 1;
			uint32_t deltaDraw = 17;

			//int l_w{ c::APP_WIN_W_INITIAL }, l_h{ c::APP_WIN_H_INITIAL };
			int l_w{ 0 }, l_h{ 0 };

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

					//input.move(realDelta, mw_used ? mouse_wheel_y : 0);
					//main_window.move(realDelta, input, config, l_w, l_h);
					main_window.move(realDelta);

					last_logic_time = tick_time;
				}

				if (deltaDraw >= 25) { // capped frame rate of ~40 is ok
					//main_window.draw(l_rnd, input, config, l_w, l_h);
					main_window.draw(l_rnd);
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
	klib::file::append_string_to_file("Runtime error. Exception was:\n" + std::string(ex.what()) + "\n", ERROR_LOG_FILE);
	return 1;
}
catch (...) {
	klib::file::append_string_to_file("Unknown runtime error occurred.\n", ERROR_LOG_FILE);
	return 1;
}


/*
constexpr unsigned int LEVEL_W{ 16 }, LEVEL_H{ 12 }, LEVEL_COUNT{ 53 };

	constexpr unsigned int OFFSET_ENEMIES{ 0x5d67 };
	constexpr unsigned int OFFSET_WALLS{ 0x603c };
	constexpr unsigned int OFFSET_GFX{ 0x8010 };

	constexpr unsigned int SIZE_LEVEL_WALL_LAYER{ (LEVEL_W * LEVEL_H) / 8 };
	constexpr unsigned int SIZE_LEVEL_WALLS{ 2 * SIZE_LEVEL_WALL_LAYER };
	constexpr unsigned int SIZE_TOTAL_WALLS{ LEVEL_COUNT * SIZE_LEVEL_WALLS };

	auto l_bytes = klib::file::read_file_as_bytes("Solomon's Key (U) [!].nes");

	std::vector<skc::Level> levels;

	for (std::size_t i{ OFFSET_WALLS }; i < OFFSET_WALLS + SIZE_TOTAL_WALLS; i += SIZE_LEVEL_WALLS) {
		std::vector<byte> l_brown_layer{
			std::vector<byte>(begin(l_bytes) + i,
			begin(l_bytes) + i + SIZE_LEVEL_WALL_LAYER) };

		std::vector<byte> l_white_layer{
	std::vector<byte>(begin(l_bytes) + i + SIZE_LEVEL_WALL_LAYER,
	begin(l_bytes) + i + 2 * SIZE_LEVEL_WALL_LAYER) };

		levels.push_back(skc::Level(l_brown_layer, l_white_layer));

	}

	for (std::size_t i{ 0 }; i < levels.size(); ++i)
		std::cout << i << "\n" << levels[i].get_blocks();

	klib::NES_Palette x({ 1, 1, 1 });

	std::vector<byte> img01v = std::vector<byte>(begin(l_bytes) + 0x8010, begin(l_bytes) + 0x8010 + 16);
	klib::NES_Gfx_tile img01(img01v);

	/*
	int lvl_no{ 1 };
	unsigned int l_idx{ OFFSET_ENEMIES };

	while (lvl_no <= 53) {
		std::cout << "Level " << lvl_no << "\n";

		while (true) {
			int a = l_bytes.at(l_idx);
			int b = l_bytes.at(l_idx + 1);
			l_idx += 2;

			if (a == 0) {
				++lvl_no;
				std::cout << "Leftover: " << b << "\n";
				break;
			}
			else {
				std::cout << "Enemy " << a << " @ " << (b % 16) << "," << (b / 16) << "\n";
			}
		}
	}
	*/