#include "SKC_Main_window.h"
#include "./../common/klib/klib_gfx.h"

constexpr unsigned int LEVEL_W{ 16 }, LEVEL_H{ 12 }, LEVEL_COUNT{ 53 };

constexpr unsigned int OFFSET_ENEMIES{ 0x5d67 };
constexpr unsigned int OFFSET_WALLS{ 0x603c };
constexpr unsigned int OFFSET_GFX{ 0x8010 };

constexpr unsigned int SIZE_LEVEL_WALL_LAYER{ (LEVEL_W * LEVEL_H) / 8 };
constexpr unsigned int SIZE_LEVEL_WALLS{ 2 * SIZE_LEVEL_WALL_LAYER };
constexpr unsigned int SIZE_TOTAL_WALLS{ LEVEL_COUNT * SIZE_LEVEL_WALLS };

skc::SKC_Main_window::SKC_Main_window(SDL_Renderer* p_rnd, const std::vector<byte>& p_bytes) :
	m_gfx{ p_rnd, p_bytes }
{
	for (std::size_t i{ OFFSET_WALLS }; i < OFFSET_WALLS + SIZE_TOTAL_WALLS; i += SIZE_LEVEL_WALLS) {
		std::vector<byte> l_brown_layer{
			std::vector<byte>(begin(p_bytes) + i,
			begin(p_bytes) + i + SIZE_LEVEL_WALL_LAYER) };

		std::vector<byte> l_white_layer{
	std::vector<byte>(begin(p_bytes) + i + SIZE_LEVEL_WALL_LAYER,
	begin(p_bytes) + i + 2 * SIZE_LEVEL_WALL_LAYER) };

		m_levels.push_back(skc::Level(l_brown_layer, l_white_layer));
	}

	byte l_pos = p_bytes.at(0x6a9d);
	m_levels.at(0).set_player_start_pos(l_pos % 16, l_pos / 16 - 1);

	l_pos = p_bytes.at(0x6ab5);
	m_levels.at(1).set_player_start_pos(l_pos % 16, l_pos / 16 - 1);

	l_pos = p_bytes.at(0x6ac6);
	m_levels.at(2).set_player_start_pos(l_pos % 16, l_pos / 16 - 1);

	l_pos = p_bytes.at(0x6ad9);
	m_levels.at(3).set_player_start_pos(l_pos % 16, l_pos / 16 - 1);

	/*
	6a9d
		+ 18
		6ab5
		+ 11
		6ac6
		+ 13
		6ad9
		*/

}

void skc::SKC_Main_window::move(int p_delta_ms) {

}

void skc::SKC_Main_window::draw(SDL_Renderer* p_rnd) {
	constexpr int TILE_SIZE_VISUAL{ 32 };
	std::size_t l_level_no{ 3 };

	// draw background
	for (int j{ 0 }; j < LEVEL_H; ++j)
		for (int i{ 0 }; i < LEVEL_W; ++i) {
			int l_tile_no{ 0 };
			auto l_ttype = m_levels.at(l_level_no).get_wall_type(i, j);
			if (l_ttype == skc::Wall::Brown)
				l_tile_no = 1;
			else if (l_ttype == skc::Wall::White || l_ttype == skc::Wall::Brown_white)
				l_tile_no = 2;

			klib::gfx::blit_scale(p_rnd, m_gfx.get_tile_gfx(l_tile_no), 20 + i * TILE_SIZE_VISUAL, 20 + j * TILE_SIZE_VISUAL, TILE_SIZE_VISUAL, TILE_SIZE_VISUAL);
		}

	// draw player start
	auto l_pstart = m_levels.at(l_level_no).get_player_start_pos();
	klib::gfx::blit_scale(p_rnd,
		m_gfx.get_tile_gfx(3),
		20 + l_pstart.first * TILE_SIZE_VISUAL,
		20 + l_pstart.second * TILE_SIZE_VISUAL, TILE_SIZE_VISUAL, TILE_SIZE_VISUAL);
}
