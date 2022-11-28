#include "SKC_Gfx.h"
#include "./common/klib/klib_gfx.h"
#include "./common/klib/klib_util.h"
#include "./skc_constants/Constants_application.h"
#include "./skc_constants/Constants_xml.h"
#include "./common/pugixml/pugixml.hpp"
#include "./common/pugixml/pugiconfig.hpp"
#include <stdexcept>

constexpr unsigned int OFFSET_GFX{ 0x8010 };

skc::SKC_Gfx::~SKC_Gfx(void) {
	for (auto l_texture : m_tile_gfx)
		if (l_texture != nullptr)
			SDL_DestroyTexture(l_texture);
}

skc::SKC_Gfx::SKC_Gfx(SDL_Renderer* p_rnd,
	const std::vector<unsigned char> p_rom_data) {

	constexpr unsigned int NES_GFX_TILE_BYTE_SIZE{ 16 };

	std::vector<byte> l_chr_data{ std::vector<byte>(begin(p_rom_data) + OFFSET_GFX, end(p_rom_data)) };

	for (std::size_t i{ 0 }; i < l_chr_data.size(); i += NES_GFX_TILE_BYTE_SIZE) {
		m_tiles.push_back(klib::NES_Gfx_tile(
			std::vector<byte>(begin(l_chr_data) + i,
				begin(l_chr_data) + i + NES_GFX_TILE_BYTE_SIZE))
		);
	}

	this->load_metadata();
	this->generate_tile_textures(p_rnd);
}

SDL_Texture* skc::SKC_Gfx::get_tile_gfx(std::size_t p_gfx_no) const {
	return m_tile_gfx.at(p_gfx_no);
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
	int p_x, int p_y) const {

	for (int j{ 0 }; j < 8; ++j)
		for (int i{ 0 }; i < 8; ++i)
			klib::gfx::put_pixel(p_surface, p_x + i, p_y + j,
				m_palettes.at(p_palette_no).get_nes_palette_index(
					m_tiles.at(p_tile_no).get_palette_index(i, j))
			);
}

SDL_Color skc::SKC_Gfx::nes_color_to_sdl(const klib::NES_Color& p_col) {
	return SDL_Color{ p_col.m_r, p_col.m_g, p_col.m_b };
}

void skc::SKC_Gfx::load_metadata(void) {
	pugi::xml_document doc;
	if (!doc.load_file(skc::c::FILENAME_CONFIG_XML))
		throw std::runtime_error("Could not load configuration xml");

	auto n_meta = doc.child(skc::c::XML_TAG_META);
	auto n_gfx_meta = n_meta.child(skc::c::XML_TAG_GFX_METADATA);
	auto n_palettes = n_gfx_meta.child(skc::c::XML_TAG_PALETTES);

	for (auto n_palette = n_palettes.child(skc::c::XML_TAG_PALETTE);
		n_palette;
		n_palette = n_palette.next_sibling(skc::c::XML_TAG_PALETTE)) {
		m_palettes.push_back(klib::NES_Palette(klib::util::string_split<byte>(
			n_palette.attribute(skc::c::XML_ATTR_COLORS).as_string(), ',')));
	}

	auto n_tile_defs = n_gfx_meta.child(skc::c::XML_TAG_TILE_DEFINITIONS);

	for (auto n_tile = n_tile_defs.child(skc::c::XML_TAG_TILE);
		n_tile;
		n_tile = n_tile.next_sibling(skc::c::XML_TAG_TILE)) {

		auto l_values = klib::util::string_split<int>(
			n_tile.attribute(skc::c::XML_ATTR_NES_TILES).as_string(), ',');
		int l_w = n_tile.attribute(skc::c::XML_ATTR_W).as_int();

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

		SKC_Tile_definition l_definition(l_w, l_h);
		for (std::size_t i{ 0 }; i < l_values.size(); ++i) {
			std::size_t l_x = i % l_w;
			std::size_t l_y = i / l_w;
			l_definition.add_tile_metadata(
				SKC_Tile_metadata(l_values.at(i),
					l_palette_no),
				l_x, l_y);
		}
		m_tile_definitions.push_back(l_definition);
	}
}

void skc::SKC_Gfx::generate_tile_textures(SDL_Renderer* p_rnd) {

	for (const auto& l_meta : m_tile_definitions) {
		auto l_srf = create_nes_sdl_surface(8 * l_meta.get_w(), 8 * l_meta.get_h());

		for (int j{ 0 }; j < l_meta.get_h(); ++j)
			for (int i{ 0 }; i < l_meta.get_h(); ++i) {
				draw_tile_on_surface(l_srf,
					l_meta.get_nes_tile_no(i, j),
					l_meta.get_palette_no(i, j),
					8 * i, 8 * j);
			}

		m_tile_gfx.push_back(klib::gfx::surface_to_texture(p_rnd, l_srf));
	}

}
