#include "SKC_Gfx.h"
#include "./common/klib/klib_gfx.h"
#include "./common/klib/klib_util.h"
#include "./skc_constants/Constants_application.h"
#include "./skc_constants/Constants_xml.h"
#include "./common/pugixml/pugixml.hpp"
#include "./common/pugixml/pugiconfig.hpp"
#include <stdexcept>

using byte = unsigned char;

skc::SKC_Gfx::~SKC_Gfx(void) {
	for (auto l_texture : m_tile_gfx)
		if (l_texture != nullptr)
			SDL_DestroyTexture(l_texture);
}

skc::SKC_Gfx::SKC_Gfx(SDL_Renderer* p_rnd,
	const SKC_Config& p_config) {
	const auto& lr_rom_data{ p_config.get_rom_data() };

	std::vector<byte> l_chr_data{ std::vector<byte>(begin(lr_rom_data) + p_config.get_offset_gfx(),
		end(lr_rom_data)) };

	for (std::size_t i{ 0 };
		i < p_config.get_nes_tile_count() * klib::c::NES_GFX_TILE_BYTE_SIZE;
		i += klib::c::NES_GFX_TILE_BYTE_SIZE) {
		m_tiles.push_back(klib::NES_Gfx_tile(
			std::vector<byte>(begin(l_chr_data) + i,
				begin(l_chr_data) + i + klib::c::NES_GFX_TILE_BYTE_SIZE))
		);
	}

	this->load_metadata(lr_rom_data);
	this->generate_tile_textures(p_rnd);

	// code for generating tilemap bmp-file
	/*
	auto l_srf = create_nes_sdl_surface(32 * 16, 16 * 16);
	std::size_t l_pal_nox{ 0 };
	for (int i{ 0 }; i < 2048; i += 4) {
		int l_x = (i/4) % 32;
		int l_y = (i/4) / 32;

		draw_tile_on_surface(l_srf, i, l_pal_nox, l_x * 16, l_y * 16);
		draw_tile_on_surface(l_srf, i + 1, l_pal_nox, l_x * 16 + 8, l_y * 16);
		draw_tile_on_surface(l_srf, i + 2, l_pal_nox, l_x * 16, l_y * 16 + 8);
		draw_tile_on_surface(l_srf, i + 3, l_pal_nox, l_x * 16 + 8, l_y * 16 + 8);
	}

	SDL_SaveBMP(l_srf, "tilemap.bmp");
	*/
}

SDL_Texture* skc::SKC_Gfx::get_item_tile(byte p_item_no, std::size_t p_tileset_no, int frame_no) const {
	const auto iter{ m_item_gfx_map.find(p_item_no) };
	return m_tile_gfx.at(iter == end(m_item_gfx_map) ? 32 : iter->second + p_tileset_no * m_tileset_tile_count);
}

SDL_Texture* skc::SKC_Gfx::get_enemy_tile(byte p_enemy_no, std::size_t p_tileset_no, int frame_no) const {
	const auto iter{ m_sprite_gfx_map.find(p_enemy_no) };
	return m_tile_gfx.at(iter == end(m_sprite_gfx_map) ? 32 : iter->second + p_tileset_no * m_tileset_tile_count);
}

SDL_Texture* skc::SKC_Gfx::get_tile_gfx(std::size_t p_gfx_no, std::size_t p_tileset_no) const {
	return m_tile_gfx.at(p_gfx_no + p_tileset_no * m_tileset_tile_count);
}

SDL_Surface* skc::SKC_Gfx::create_nes_sdl_surface(int p_w, int p_h) const {
	SDL_Surface* l_bmp = SDL_CreateRGBSurface(0, p_w,
		p_h, 8, 0, 0, 0, 0);

	SDL_Color out_palette[256] = { 0, 0, 0 };
	for (int i{ 0 }; i < static_cast<int>(klib::NES_Palette::get_nes_color_count()); ++i)
		out_palette[i] = nes_color_to_sdl(klib::NES_Palette::get_nes_color(i));

	SDL_SetPaletteColors(l_bmp->format->palette, out_palette, 0, 256);

	return l_bmp;
}

void skc::SKC_Gfx::draw_tile_on_surface(SDL_Surface* p_surface,
	std::size_t p_tile_no, std::size_t p_palette_no,
	int p_x, int p_y,
	bool p_flip_v, bool p_flip_h,
	bool p_skip_transp, bool p_global_transp) const {

	for (int j{ 0 }; j < 8; ++j)
		for (int i{ 0 }; i < 8; ++i) {
			byte l_pal_index = m_tiles.at(p_tile_no).get_palette_index(i, j, p_flip_v, p_flip_h);
			bool l_transp = (l_pal_index == 0);

			if (!l_transp || !p_skip_transp) {
				klib::gfx::put_pixel(p_surface, p_x + i, p_y + j,
					l_transp && p_global_transp ?
					klib::NES_Palette::get_transparent_index() :
					m_palettes.at(p_palette_no).get_nes_palette_index(l_pal_index)
				);
			}
		}
}

SDL_Color skc::SKC_Gfx::nes_color_to_sdl(const klib::NES_Color& p_col) {
	return SDL_Color{ p_col.m_r, p_col.m_g, p_col.m_b };
}

void skc::SKC_Gfx::load_metadata(const std::vector<byte> p_rom_data) {
	pugi::xml_document doc;
	if (!doc.load_file(skc::c::FILENAME_CONFIG_XML))
		throw std::runtime_error("Could not load configuration xml");

	auto n_meta = doc.child(skc::c::XML_TAG_META);

	auto n_enemy_defs = n_meta.child(c::XML_TAG_ENEMY_DEFINITIONS);
	for (auto n_enemy = n_enemy_defs.child(c::XML_TAG_ENEMY);
		n_enemy;
		n_enemy = n_enemy.next_sibling(c::XML_TAG_ENEMY)) {
		byte l_index{ klib::util::string_to_numeric<byte>(n_enemy.attribute(c::XML_ATTR_NO).as_string()) };
		auto l_animation{ klib::util::string_split<std::size_t>(
			n_enemy.attribute(c::XML_ATTR_ANIMATION).as_string(),
			',') };

		m_sprite_gfx_map.insert(std::make_pair(l_index, l_animation.at(0)));
	}

	auto n_item_defs = n_meta.child(c::XML_TAG_ITEM_DEFINITIONS);
	for (auto n_item = n_item_defs.child(c::XML_TAG_ITEM);
		n_item;
		n_item = n_item.next_sibling(c::XML_TAG_ITEM)) {
		byte l_index{ klib::util::string_to_numeric<byte>(n_item.attribute(c::XML_ATTR_NO).as_string()) };
		auto l_animation{ klib::util::string_split<std::size_t>(
			n_item.attribute(c::XML_ATTR_ANIMATION).as_string(),
			',') };

		m_item_gfx_map.insert(std::make_pair(l_index, l_animation.at(0)));
	}

	auto n_gfx_meta = n_meta.child(skc::c::XML_TAG_GFX_METADATA);
	auto n_palettes = n_gfx_meta.child(skc::c::XML_TAG_PALETTES);

	for (auto n_palette = n_palettes.child(skc::c::XML_TAG_PALETTE);
		n_palette;
		n_palette = n_palette.next_sibling(skc::c::XML_TAG_PALETTE)) {

		auto l_pal_data = klib::util::string_split<std::size_t>(
			n_palette.attribute(skc::c::XML_ATTR_OFFSET).as_string(), ','
			);
		if (l_pal_data.size() == 1)
			m_palettes.push_back(klib::NES_Palette(std::vector<byte>(begin(p_rom_data) + l_pal_data[0],
				begin(p_rom_data) + l_pal_data[0] + 4)));
		else
			m_palettes.push_back(klib::NES_Palette(l_pal_data));
	}

	// loop over all tilesets, and generate all tiles for each iteration
	std::size_t l_tileset_count{ 0 };
	auto n_tilesets = n_gfx_meta.child(c::XML_TAG_TILESETS);
	for (auto n_tileset = n_tilesets.child(c::XML_TAG_TILESET); n_tileset;
		n_tileset = n_tileset.next_sibling(c::XML_TAG_TILESET)) {
		++l_tileset_count;
		unsigned int l_palette_offset =
			klib::util::string_to_numeric<unsigned int>(n_tileset.attribute(c::XML_ATTR_PALETTE_OFFSET).as_string());
		unsigned int l_tile_offset =
			klib::util::string_to_numeric<unsigned int>(n_tileset.attribute(c::XML_ATTR_TILE_OFFSET).as_string());

		auto n_tile_defs = n_gfx_meta.child(skc::c::XML_TAG_TILE_DEFINITIONS);

		for (auto n_tile = n_tile_defs.child(skc::c::XML_TAG_TILE);
			n_tile;
			n_tile = n_tile.next_sibling(skc::c::XML_TAG_TILE)) {
			auto l_values = klib::util::string_split_strings(
				n_tile.attribute(skc::c::XML_ATTR_NES_TILES).as_string(), ',');
			int l_w = n_tile.attribute(skc::c::XML_ATTR_W).as_int();
			bool l_transparent = n_tile.attribute(c::XML_ATTR_TRANSPARENT).as_bool();

			// if w is not given, determine the value automatically
			// if the tile consists of 4 nes tiles, we assume 2x2 tile,
			// otherwise we assume a horizontal strip of the given tiles
			if (l_w == 0) {
				if (l_values.size() == 4)
					l_w = 2;
				else
					l_w = static_cast<int>(l_values.size());
			}
			int l_h = static_cast<int>(l_values.size()) / l_w;
			std::size_t l_palette_no = n_tile.attribute(skc::c::XML_ATTR_PALETTE_NO).as_int();

			SKC_Tile_definition l_definition(l_w, l_h, l_transparent);
			for (std::size_t i{ 0 }; i < l_values.size(); ++i) {
				std::size_t l_x = i % l_w;
				std::size_t l_y = i / l_w;

				auto l_unpack = klib::util::string_split_strings(l_values.at(i), ':');
				int l_unpack_tile_no{ klib::util::string_to_numeric<int>(l_unpack.at(0)) };
				bool l_flip_v{ false }, l_flip_h{ false };
				if (l_unpack.size() > 1) {
					char l_flip_instruction{ l_unpack[1].at(0) };
					if (l_flip_instruction == 'r') {
						l_flip_v = true;
						l_flip_h = true;
					}
					else if (l_flip_instruction == 'v')
						l_flip_v = true;
					else if (l_flip_instruction == 'h')
						l_flip_h = true;
				}

				l_definition.add_tile_metadata(
					SKC_Tile_metadata(l_unpack_tile_no + l_tile_offset,
						l_palette_no + l_palette_offset, l_flip_v, l_flip_h),
					l_x, l_y);
			}
			m_tile_definitions.push_back(l_definition);
		}

	}

	m_tileset_tile_count = m_tile_definitions.size() / l_tileset_count;
}

void skc::SKC_Gfx::generate_tile_textures(SDL_Renderer* p_rnd) {

	for (const auto& l_meta : m_tile_definitions) {
		auto l_srf = create_nes_sdl_surface(8 * l_meta.get_w(), 8 * l_meta.get_h());

		for (int j{ 0 }; j < l_meta.get_h(); ++j)
			for (int i{ 0 }; i < l_meta.get_h(); ++i) {
				draw_tile_on_surface(l_srf,
					l_meta.get_nes_tile_no(i, j),
					l_meta.get_palette_no(i, j),
					8 * i, 8 * j,
					l_meta.is_flip_v(i, j),
					l_meta.is_flip_h(i, j),
					false, l_meta.is_transparent());
			}

		if (l_meta.is_transparent()) {
			SDL_Color l_trans_rgb = nes_color_to_sdl(klib::NES_Palette::get_transparent_color());
			SDL_SetColorKey(l_srf, true, SDL_MapRGB(l_srf->format, l_trans_rgb.r, l_trans_rgb.g, l_trans_rgb.b));
		}

		m_tile_gfx.push_back(klib::gfx::surface_to_texture(p_rnd, l_srf));
	}

}
