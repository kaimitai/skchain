#include "SKC_Main_window.h"
#include "./../common/klib/klib_gfx.h"
#include "./../skc_util/Xml_helper.h"
#include <map>
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

	// DEBUG
	// #include "./../common/klib/klib_file.h"
	/*
	std::vector<byte> l_generated_item_bytes;
	std::vector<byte> l_original_item_bytes(begin(p_bytes) + l_item_offsets.at(0),
		begin(p_bytes) + l_item_offsets.at(0) + 1300);
	for (std::size_t i{ 0 }; i < m_levels.size(); ++i) {
		std::string l_filename = "level-" + std::to_string(i) + ".xml";
		skc::save_level_xml(m_levels.at(i), "./xml", l_filename);
		auto l_lvl_item_bytes{ m_levels[i].get_item_bytes() };
		//klib::file::write_bytes_to_file(l_lvl_item_bytes, "./dat/test-items-" + std::to_string(i) + ".dat");
		l_generated_item_bytes.insert(end(l_generated_item_bytes), begin(l_lvl_item_bytes), end(l_lvl_item_bytes));
	}
	//klib::file::write_bytes_to_file(l_original_item_bytes, "./dat/test-items-original.dat");
	//klib::file::write_bytes_to_file(l_generated_item_bytes, "./dat/test-items.dat");
	*/
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

	// draw door
	auto l_door_pos = m_levels.at(m_current_level).get_door_pos();
	if (l_door_pos.second >= 0)
		klib::gfx::blit_scale(p_rnd,
			m_gfx.get_tile_gfx(4),
			20 + l_door_pos.first * TILE_SIZE_VISUAL,
			20 + l_door_pos.second * TILE_SIZE_VISUAL, TILE_SIZE_VISUAL, TILE_SIZE_VISUAL);

	// draw player start
	auto l_pstart = m_levels.at(m_current_level).get_player_start_pos();
	klib::gfx::blit_scale(p_rnd,
		m_gfx.get_tile_gfx(3),
		20 + l_pstart.first * TILE_SIZE_VISUAL,
		20 + l_pstart.second * TILE_SIZE_VISUAL, TILE_SIZE_VISUAL, TILE_SIZE_VISUAL);

	// draw key
	auto l_key_pos = m_levels.at(m_current_level).get_key_pos();
	if (l_key_pos.second >= 0)
		klib::gfx::blit_scale(p_rnd,
			m_gfx.get_tile_gfx(5),
			20 + l_key_pos.first * TILE_SIZE_VISUAL,
			20 + l_key_pos.second * TILE_SIZE_VISUAL, TILE_SIZE_VISUAL, TILE_SIZE_VISUAL);

	static std::map<byte, std::size_t> m_item_tiles{
		{0x04, 6}, {0x33,7}, {0x18, 8}, {0x2b, 9},
		{0x08, 10}, {0x0c, 11}, {0x15, 12}, {0x16, 13},
		{0x13, 14}, {0x14, 15}, {0x22, 16}, {0x1b, 17}, {0x0b, 18},
		{0x27, 19}, {0x2a, 20}, {0x2c, 20}, {0x2d, 20}, {0x25, 21}, {0x28, 22},
		{0x12, 23}, {0x19, 24},
		{0x1c, 25}, {0x1d, 26}, {0x1e, 27}, {0x1f, 28},
		{0x20, 29}, {0x21, 30}, {0x2f, 31},
		{0x0e, 33}, {0x17, 33},
		{0x26, 34}, {0x29, 35},
		{0x2e, 36}, {0x11, 37}, {0x30, 38}, {0x31, 30}, {0x32, 39}
	};

	// draw items
	const auto& l_items = m_levels.at(m_current_level).get_elements();
	for (const auto& item : l_items) {
		byte l_no = item.get_item_no();
		if (item.get_element_type() == skc::Element_type::Item) {
			auto l_pos = item.get_position();
			auto l_iter = m_item_tiles.find(l_no);
			bool l_found = (l_iter != end(m_item_tiles));

			klib::gfx::blit_scale(p_rnd,
				m_gfx.get_tile_gfx(l_found ? l_iter->second : 32),
				20 + l_pos.first * TILE_SIZE_VISUAL,
				20 + l_pos.second * TILE_SIZE_VISUAL, TILE_SIZE_VISUAL, TILE_SIZE_VISUAL);
		}
	}

	static std::map<byte, std::size_t> m_enemy_tiles{
		{0x70, 40}, {0x71, 40},{0x72, 40},{0x73, 40}, {0x74, 40}, {0x75, 40},
		{0x68, 41}, {0x6d, 41}, {0x69, 41},
		{0x1c, 42},
		{0x30, 43}, {0x31, 43},{0x32, 43},{0x33, 43},{0x34, 43},{0x35, 43},{0x36, 43},
		{0x37, 43}, {0x38, 43},{0x39, 43},{0x3a, 43},{0x3b, 43},{0x3c, 43},{0x3d, 43},{0x3e, 43},{0x3f, 43},
		{0x40, 43}, {0x41, 43},{0x42, 43},{0x43, 43},
		{0x44, 43}, {0x45, 43},{0x46, 43},{0x47, 43},{0x48, 43},{0x49, 43},{0x4a, 43},{0x4b, 43},
		{0x4c, 43}, {0x4d, 43},{0x4e, 43},{0x4f, 43},
		{0x20, 44}, {0x21, 44},{0x22, 44},{0x23, 44},
		{0x24, 45}, {0x25, 45},{0x26, 45},{0x27, 45},
		{0x28, 44}, {0x29, 44},{0x2a, 44},{0x2b, 44}, {0x2c, 44}, {0x2d, 44},{0x2e, 44},{0x2f, 44},
		{0x78, 46}, {0x79, 46}, {0x7d, 46},
		{0x80, 48}, {0x81, 47}, // red and white flames
		{0x1d, 49} // princess
	};

	// draw enemies
	const auto& l_enemies = m_levels.at(m_current_level).get_elements();
	for (const auto& enemy : l_enemies) {
		byte l_no = enemy.get_element_no();
		if (enemy.get_element_type() == skc::Element_type::Enemy) {
			auto l_pos = enemy.get_position();
			auto l_iter = m_enemy_tiles.find(l_no);
			bool l_found = (l_iter != end(m_enemy_tiles));

			klib::gfx::blit_scale(p_rnd,
				m_gfx.get_tile_gfx(l_found ? l_iter->second : 32),
				20 + l_pos.first * TILE_SIZE_VISUAL,
				20 + l_pos.second * TILE_SIZE_VISUAL, TILE_SIZE_VISUAL, TILE_SIZE_VISUAL);
		}
	}
}
