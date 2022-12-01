#ifndef SKC_MAIN_WINDOW_H
#define SKC_MAIN_WINDOW_H

#include <SDL.h>
#include <vector>
#include "./../common/klib/User_input.h"
#include "./../SKC_Config.h"
#include "./../SKC_Level.h"
#include "./../SKC_Gfx.h"

using byte = unsigned char;

namespace skc {

	class SKC_Main_window {
		SKC_Gfx m_gfx;
		std::vector<skc::Level> m_levels;
		std::size_t m_current_level;

	public:
		SKC_Main_window(SDL_Renderer* p_rnd, const SKC_Config& p_config);
		void move(int p_delta_ms, const klib::User_input& p_input);
		void draw(SDL_Renderer* p_rnd, const SKC_Config& p_config);

		void save_nes_file(const std::string& p_file_path, const SKC_Config& p_config) const;
	};

}

#endif
