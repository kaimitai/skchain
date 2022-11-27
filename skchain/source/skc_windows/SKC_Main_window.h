#ifndef SKC_MAIN_WINDOW_H
#define SKC_MAIN_WINDOW_H

#include <SDL.h>
#include <vector>
#include "./../SKC_Level.h"
#include "./../SKC_Gfx.h"

using byte = unsigned char;

namespace skc {

	class SKC_Main_window {
		SKC_Gfx m_gfx;
		std::vector<skc::Level> m_levels;

	public:
		SKC_Main_window(SDL_Renderer* p_rnd, const std::vector<byte>& p_bytes);
		void move(int p_delta_ms);
		void draw(SDL_Renderer* p_rnd);
	};

}

#endif
