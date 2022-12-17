#ifndef SKC_GFX_H
#define SKC_GFX_H

#include <SDL.h>
#include <map>
#include <vector>
#include "./common/klib/NES_Gfx_tile.h"
#include "./common/klib/NES_Palette.h"
#include "SKC_Tile_metadata.h"
#include "SKC_Config.h"

using byte = unsigned char;

namespace skc {

	class SKC_Gfx {

		SDL_Surface* create_nes_sdl_surface(int p_w, int p_h) const;
		void draw_tile_on_surface(SDL_Surface* p_surface,
			std::size_t p_tile_no, std::size_t p_palette_no,
			int p_x, int p_y,
			bool p_flip_v = false, bool p_flip_h = false,
			bool p_skip_transp = false, bool p_global_transp = false) const;

		std::vector<klib::NES_Gfx_tile> m_tiles;
		std::vector<skc::SKC_Tile_definition> m_tile_definitions;
		std::vector<SDL_Texture*> m_tile_gfx;
		std::vector<SDL_Texture*> m_hex_gfx;
		std::vector<klib::NES_Palette> m_palettes;
		std::size_t m_tileset_tile_count;

		std::map<byte, std::size_t> m_sprite_gfx_map, m_item_gfx_map, m_meta_gfx_map;

		SDL_Surface* generate_tile_surface(const SKC_Tile_definition& p_tile_def) const;
		void load_metadata(const std::string& p_config_file_path, const std::vector<byte> p_rom_data);
		void generate_tile_textures(SDL_Renderer* p_rnd);
		void generate_hex_textures(SDL_Renderer* p_rnd);

		SDL_Texture* get_gfx_translated(std::size_t p_tile_no, std::size_t p_tileset_no) const;

	public:
		static SDL_Color nes_color_to_sdl(const klib::NES_Color& p_col);

		~SKC_Gfx(void);
		SKC_Gfx(SDL_Renderer* p_rnd, const SKC_Config& p_config);
		SDL_Texture* get_meta_tile(byte p_meta_no, std::size_t p_tileset_no = 0, int p_frame_no = 0) const;
		SDL_Texture* get_enemy_tile(byte p_enemy_no, std::size_t p_tileset_no = 0, int p_frame_no = 0) const;
		SDL_Texture* get_item_tile(byte p_item_no, std::size_t p_tilset_no = 0, int p_frame_no = 0) const;
		SDL_Texture* get_tile(std::size_t p_element_type, byte p_item_no, std::size_t p_tileset_no, int p_frame_no = 0) const;
		SDL_Texture* get_absolute_tile(std::size_t p_tile_no, std::size_t p_tileset_no) const;
		void set_application_icon(SDL_Window* p_window) const;
	};
}

#endif
