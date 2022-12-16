#ifndef SKC_CONSTANTS_COLOR_H
#define SKC_CONSTANTS_COLOR_H

#include <SDL.h>
#include "./../common/imgui/imgui.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"

using byte = unsigned char;

namespace skc {

	namespace c {

		constexpr ImVec4 sdl_to_imgui(SDL_Color p_col) {
			return ImVec4{ static_cast<float>(p_col.r) / 255.0f,
				static_cast<float>(p_col.g) / 255.0f,
				static_cast<float>(p_col.b) / 255.0f,
				1.0f };
		}

		constexpr SDL_Color COL_GREEN{ 0,255,0 };
		constexpr SDL_Color COL_RED{ 255,0,0 };
		constexpr SDL_Color COL_YELLOW{ 255,255,0 };
		constexpr SDL_Color COL_WHITE{ 255,255,255 };

		constexpr ImVec4 COLI_GREEN{ sdl_to_imgui(COL_GREEN) };
		constexpr ImVec4 COLI_RED{ sdl_to_imgui(COL_RED) };
		constexpr ImVec4 COLI_YELLOW{ sdl_to_imgui(COL_YELLOW) };
		constexpr ImVec4 COLI_WHITE{ sdl_to_imgui(COL_WHITE) };
	}

}

#endif
