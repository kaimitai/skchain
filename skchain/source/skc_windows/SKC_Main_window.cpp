#include "SKC_Main_window.h"
#include "./../common/klib/klib_gfx.h"
#include <vector>

constexpr unsigned int LEVEL_W{ 16 }, LEVEL_H{ 12 }, LEVEL_COUNT{ 53 };

constexpr unsigned int OFFSET_ENEMIES{ 0x5d67 };
constexpr unsigned int OFFSET_WALLS{ 0x603c };
constexpr unsigned int OFFSET_GFX{ 0x8010 };

constexpr unsigned int SIZE_LEVEL_WALL_LAYER{ (LEVEL_W * LEVEL_H) / 8 };
constexpr unsigned int SIZE_LEVEL_WALLS{ 2 * SIZE_LEVEL_WALL_LAYER };
constexpr unsigned int SIZE_TOTAL_WALLS{ LEVEL_COUNT * SIZE_LEVEL_WALLS };

constexpr size_t OFFSET_ENEMY_TABLE_LO{ 0x5cfc };
constexpr size_t OFFSET_ENEMY_TABLE_HI{ OFFSET_ENEMY_TABLE_LO + LEVEL_COUNT };

constexpr size_t OFFSET_ITEM_TABLE_LO{ 0x6a2c };
constexpr size_t OFFSET_ITEM_TABLE_HI{ OFFSET_ITEM_TABLE_LO + LEVEL_COUNT };

skc::SKC_Main_window::SKC_Main_window(SDL_Renderer* p_rnd, const std::vector<byte>& p_bytes) :
	m_gfx{ p_rnd, p_bytes }, m_current_level{ 0 }
{
	m_levels = std::vector<skc::Level>(LEVEL_COUNT, skc::Level());
	std::vector<size_t> l_item_offsets, l_enemy_offsets;
	for (std::size_t i{ 0 }; i < LEVEL_COUNT; ++i) {
		std::size_t l_item_offset = p_bytes.at(OFFSET_ITEM_TABLE_HI + i) * 256 +
			p_bytes.at(OFFSET_ITEM_TABLE_LO + i);
		std::size_t l_enemy_offset = p_bytes.at(OFFSET_ENEMY_TABLE_HI + i) * 256 +
			p_bytes.at(OFFSET_ENEMY_TABLE_LO + i);
		l_item_offsets.push_back(l_item_offset - 0x8000 + 0x10);
		l_enemy_offsets.push_back(l_enemy_offset - 0x8000 + 0x10);
	}

	for (std::size_t i{ 0 }; i < m_levels.size(); ++i) {
		m_levels.at(i).load_block_data(p_bytes, OFFSET_WALLS + i * SIZE_LEVEL_WALLS);
		m_levels.at(i).load_item_data(p_bytes, l_item_offsets.at(i));
		m_levels.at(i).load_enemy_data(p_bytes, l_enemy_offsets.at(i));
	}
}

void skc::SKC_Main_window::move(int p_delta_ms, const klib::User_input& p_input) {
	if (p_input.mw_up() && m_current_level < m_levels.size() - 1)
		++m_current_level;
	else if (p_input.mw_down() && m_current_level > 0)
		--m_current_level;
}

void skc::SKC_Main_window::draw(SDL_Renderer* p_rnd) {
	constexpr int TILE_SIZE_VISUAL{ 32 };

	// draw background
	for (int j{ 0 }; j < LEVEL_H; ++j)
		for (int i{ 0 }; i < LEVEL_W; ++i) {
			int l_tile_no{ 0 };
			auto l_ttype = m_levels.at(m_current_level).get_wall_type(i, j);
			if (l_ttype == skc::Wall::Brown)
				l_tile_no = 1;
			else if (l_ttype == skc::Wall::White || l_ttype == skc::Wall::Brown_white)
				l_tile_no = 2;

			klib::gfx::blit_scale(p_rnd, m_gfx.get_tile_gfx(l_tile_no), 20 + i * TILE_SIZE_VISUAL, 20 + j * TILE_SIZE_VISUAL, TILE_SIZE_VISUAL, TILE_SIZE_VISUAL);
		}

	// draw player start
	auto l_pstart = m_levels.at(m_current_level).get_player_start_pos();
	klib::gfx::blit_scale(p_rnd,
		m_gfx.get_tile_gfx(3),
		20 + l_pstart.first * TILE_SIZE_VISUAL,
		20 + l_pstart.second * TILE_SIZE_VISUAL, TILE_SIZE_VISUAL, TILE_SIZE_VISUAL);
}
