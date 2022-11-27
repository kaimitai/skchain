#include "SKC_Gfx.h"
#include "./common/klib/klib_gfx.h"

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

	klib::NES_Palette l_pal_dana({ 0x26,0x29,0x30 });
	klib::NES_Palette l_pal_wall_white({ 0x07,0x10,0x30 });
	klib::NES_Palette l_pal_wall_brown({ 0x07,0x27,0x30 });

	for (int i = 0; i < m_tiles.size() / 4 - 1; ++i) {
		int l_offset = i * 4;

		auto l_srf = create_nes_sdl_surface(16, 16);
		draw_tile_on_surface(l_srf, l_offset + 0, l_pal_wall_brown, 0, 0);
		draw_tile_on_surface(l_srf, l_offset + 2, l_pal_wall_brown, 8, 0);
		draw_tile_on_surface(l_srf, l_offset + 1, l_pal_wall_brown, 0, 8);
		draw_tile_on_surface(l_srf, l_offset + 3, l_pal_wall_brown, 8, 8);

		m_tile_gfx.push_back(klib::gfx::surface_to_texture(p_rnd, l_srf));
	}

	{
		auto l_srf = create_nes_sdl_surface(16, 16);
		int l_offset = 0;
		draw_tile_on_surface(l_srf, l_offset, l_pal_dana, 0, 0);
		draw_tile_on_surface(l_srf, l_offset + 2, l_pal_dana, 8, 0);
		draw_tile_on_surface(l_srf, l_offset + 1, l_pal_dana, 0, 8);
		draw_tile_on_surface(l_srf, l_offset + 3, l_pal_dana, 8, 8);

		m_tile_gfx.insert(begin(m_tile_gfx), klib::gfx::surface_to_texture(p_rnd, l_srf));
	}

	{
		auto l_srf = create_nes_sdl_surface(16, 16);
		int l_offset = 4 * (30 * 3 + 7);
		draw_tile_on_surface(l_srf, l_offset, l_pal_wall_white, 0, 0);
		draw_tile_on_surface(l_srf, l_offset + 1, l_pal_wall_white, 8, 0);
		draw_tile_on_surface(l_srf, l_offset + 2, l_pal_wall_white, 0, 8);
		draw_tile_on_surface(l_srf, l_offset + 3, l_pal_wall_white, 8, 8);

		m_tile_gfx.insert(begin(m_tile_gfx), klib::gfx::surface_to_texture(p_rnd, l_srf));
	}

	{
		auto l_srf = create_nes_sdl_surface(16, 16);
		int l_offset = 4 * (30 * 3 + 10);
		draw_tile_on_surface(l_srf, l_offset, l_pal_wall_brown, 0, 0);
		draw_tile_on_surface(l_srf, l_offset + 1, l_pal_wall_brown, 8, 0);
		draw_tile_on_surface(l_srf, l_offset + 2, l_pal_wall_brown, 0, 8);
		draw_tile_on_surface(l_srf, l_offset + 3, l_pal_wall_brown, 8, 8);

		m_tile_gfx.insert(begin(m_tile_gfx), klib::gfx::surface_to_texture(p_rnd, l_srf));
	}

	{
		auto l_srf = create_nes_sdl_surface(16, 16);
		int l_offset = 4 * (30 * 2 + 15);
		draw_tile_on_surface(l_srf, l_offset, l_pal_wall_brown, 0, 0);
		draw_tile_on_surface(l_srf, l_offset + 1, l_pal_wall_brown, 8, 0);
		draw_tile_on_surface(l_srf, l_offset + 2, l_pal_wall_brown, 0, 8);
		draw_tile_on_surface(l_srf, l_offset + 3, l_pal_wall_brown, 8, 8);

		m_tile_gfx.insert(begin(m_tile_gfx), klib::gfx::surface_to_texture(p_rnd, l_srf));
	}

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
	std::size_t p_tile_no, const klib::NES_Palette& p_palette,
	int p_x, int p_y) const {

	for (int j{ 0 }; j < 8; ++j)
		for (int i{ 0 }; i < 8; ++i)
			klib::gfx::put_pixel(p_surface, p_x + i, p_y + j,
				p_palette.get_nes_palette_index(
					m_tiles.at(p_tile_no).get_palette_index(i, j))
			);

}

SDL_Color skc::SKC_Gfx::nes_color_to_sdl(const klib::NES_Color& p_col) {
	return SDL_Color{ p_col.m_r, p_col.m_g, p_col.m_b };
}
