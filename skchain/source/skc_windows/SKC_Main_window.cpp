#include "SKC_Main_window.h"
#include "./../common/klib/klib_gfx.h"
#include "./../skc_util/Xml_helper.h"
#include "./../skc_constants/Constants_level.h"
#include <map>
#include <vector>

skc::SKC_Main_window::SKC_Main_window(SDL_Renderer* p_rnd, const SKC_Config& p_config) :
	m_gfx{ p_rnd, p_config }, m_current_level{ 0 }
{
	const auto& lr_rom_data{ p_config.get_rom_data() };

	m_levels = std::vector<skc::Level>(p_config.get_level_count(), skc::Level());
	std::vector<size_t> l_item_offsets, l_enemy_offsets;
	for (std::size_t i{ 0 }; i < p_config.get_level_count(); ++i) {
		std::size_t l_item_offset = lr_rom_data.at(p_config.get_offset_item_table_hi() + i) * 256 +
			lr_rom_data.at(p_config.get_offset_item_table_lo() + i);
		std::size_t l_enemy_offset = lr_rom_data.at(p_config.get_offset_enemy_table_hi() + i) * 256 +
			lr_rom_data.at(p_config.get_offset_enemy_table_lo() + i);
		l_item_offsets.push_back(p_config.get_rom_address_from_ram(l_item_offset));
		l_enemy_offsets.push_back(p_config.get_rom_address_from_ram(l_enemy_offset));
	}

	for (std::size_t i{ 0 }; i < m_levels.size(); ++i) {
		m_levels.at(i).load_block_data(lr_rom_data, p_config.get_offset_block_data() + i * c::SIZE_LEVEL_WALLS);
		m_levels.at(i).load_item_data(lr_rom_data, l_item_offsets.at(i));
		m_levels.at(i).load_enemy_data(lr_rom_data, l_enemy_offsets.at(i));
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

void skc::SKC_Main_window::draw(SDL_Renderer* p_rnd, const SKC_Config& p_config) {
	constexpr int TILE_SIZE_VISUAL{ 48 };
	std::size_t l_tileset_no{ p_config.get_level_tileset(m_current_level) };

	// draw background
	for (int j{ 0 }; j < c::LEVEL_H; ++j)
		for (int i{ 0 }; i < c::LEVEL_W; ++i) {
			int l_tile_no{ 0 };
			auto l_ttype = m_levels.at(m_current_level).get_wall_type(i, j);
			if (l_ttype == skc::Wall::Brown)
				l_tile_no = 1;
			else if (l_ttype == skc::Wall::White || l_ttype == skc::Wall::Brown_white)
				l_tile_no = 2;

			klib::gfx::blit_scale(p_rnd, m_gfx.get_tile_gfx(l_tile_no, l_tileset_no), 20 + i * TILE_SIZE_VISUAL, 20 + j * TILE_SIZE_VISUAL, TILE_SIZE_VISUAL, TILE_SIZE_VISUAL);
		}

	// draw door
	auto l_door_pos = m_levels.at(m_current_level).get_door_pos();
	if (l_door_pos.second >= 0)
		klib::gfx::blit_scale(p_rnd,
			m_gfx.get_tile_gfx(4, l_tileset_no),
			20 + l_door_pos.first * TILE_SIZE_VISUAL,
			20 + l_door_pos.second * TILE_SIZE_VISUAL, TILE_SIZE_VISUAL, TILE_SIZE_VISUAL);

	// draw player start
	auto l_pstart = m_levels.at(m_current_level).get_player_start_pos();
	klib::gfx::blit_scale(p_rnd,
		m_gfx.get_tile_gfx(3, l_tileset_no),
		20 + l_pstart.first * TILE_SIZE_VISUAL,
		20 + l_pstart.second * TILE_SIZE_VISUAL, TILE_SIZE_VISUAL, TILE_SIZE_VISUAL);

	// draw key
	auto l_key_pos = m_levels.at(m_current_level).get_key_pos();
	if (l_key_pos.second >= 0)
		klib::gfx::blit_scale(p_rnd,
			m_gfx.get_tile_gfx(5, l_tileset_no),
			20 + l_key_pos.first * TILE_SIZE_VISUAL,
			20 + l_key_pos.second * TILE_SIZE_VISUAL, TILE_SIZE_VISUAL, TILE_SIZE_VISUAL);

	// draw items
	const auto& l_items = m_levels.at(m_current_level).get_elements();
	for (const auto& item : l_items) {
		byte l_no = item.get_item_no();
		if (item.get_element_type() == skc::Element_type::Item) {
			auto l_pos = item.get_position();

			klib::gfx::blit_scale(p_rnd,
				m_gfx.get_item_tile(item.get_item_no(), l_tileset_no),
				20 + l_pos.first * TILE_SIZE_VISUAL,
				20 + l_pos.second * TILE_SIZE_VISUAL, TILE_SIZE_VISUAL, TILE_SIZE_VISUAL);
		}
	}

	// draw enemies
	const auto& l_enemies = m_levels.at(m_current_level).get_elements();
	for (const auto& enemy : l_enemies) {
		byte l_no = enemy.get_element_no();
		if (enemy.get_element_type() == skc::Element_type::Enemy) {
			auto l_pos = enemy.get_position();

			klib::gfx::blit_scale(p_rnd,
				m_gfx.get_enemy_tile(enemy.get_element_no(), l_tileset_no),
				20 + l_pos.first * TILE_SIZE_VISUAL,
				20 + l_pos.second * TILE_SIZE_VISUAL, TILE_SIZE_VISUAL, TILE_SIZE_VISUAL);
		}
	}

	// draw constellation
	if (m_levels.at(m_current_level).has_constellation()) {
		auto l_pos = m_levels.at(m_current_level).get_constellation_pos();
		klib::gfx::blit_scale(p_rnd,
			m_gfx.get_constellation_gfx(m_levels.at(m_current_level).get_constellation_no() % 4,
				l_tileset_no),
			20 + l_pos.first * TILE_SIZE_VISUAL,
			20 + l_pos.second * TILE_SIZE_VISUAL, 3 * TILE_SIZE_VISUAL, 2 * TILE_SIZE_VISUAL);
	}
}
