#ifndef SKC_GFX_H
#define SKC_GFX_H

#include <SDL.h>
#include <vector>
#include "./common/klib/NES_Gfx_tile.h"
#include "./common/klib/NES_Palette.h"

using byte = unsigned char;

namespace skc {

	class SKC_Gfx {

		SDL_Surface* create_nes_sdl_surface(int p_w, int p_h) const;
		void draw_tile_on_surface(SDL_Surface* p_surface,
			std::size_t p_tile_no, std::size_t p_palette_no,
			int p_x, int p_y) const;

		std::vector<klib::NES_Gfx_tile> m_tiles;
		std::vector<SDL_Texture*> m_tile_gfx;
		std::vector<klib::NES_Palette> m_palettes;

		void load_metadata(void);

	public:
		static SDL_Color nes_color_to_sdl(const klib::NES_Color& p_col);

		~SKC_Gfx(void);
		SKC_Gfx(SDL_Renderer* p_rnd,
			const std::vector<unsigned char> p_chr_data);
		SDL_Texture* get_tile_gfx(std::size_t p_gfx_no) const;
	};
}

#endif
