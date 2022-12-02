#include "SKC_Main_window.h"
#include "./../common/klib/klib_gfx.h"
#include "./../common/klib/klib_file.h"
#include "./../skc_util/Xml_helper.h"
#include "./../skc_constants/Constants_level.h"
#include "./../common/imgui/imgui.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"
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

	// save_nes_file("sk_test.nes", p_config);
}

void skc::SKC_Main_window::move(int p_delta_ms, const klib::User_input& p_input) {
	if (p_input.mw_up() && m_current_level < m_levels.size() - 1)
		++m_current_level;
	else if (p_input.mw_down() && m_current_level > 0)
		--m_current_level;
}

void skc::SKC_Main_window::draw(SDL_Renderer* p_rnd, const SKC_Config& p_config) {
	constexpr int TILE_SIZE_VISUAL{ 48 };

	constexpr byte MD_BYTE_NO_KEY{ 0x00 };
	constexpr byte MD_BYTE_NO_DOOR{ 0x01 };
	constexpr byte MD_BYTE_NO_PLAYER_START{ 0x02 };
	constexpr byte MD_BYTE_NO_SPAWN01{ 0x03 };
	constexpr byte MD_BYTE_NO_SPAWN02{ 0x04 };
	constexpr byte MD_BYTE_NO_EMPTY_TILE{ 0x05 };
	constexpr byte MD_BYTE_NO_BLOCK_BROWN{ 0x06 };
	constexpr byte MD_BYTE_NO_BLOCK_WHITE{ 0x07 };
	constexpr byte MD_BYTE_NO_PLAYER_BLOCK_BW{ 0x08 };

	std::size_t l_tileset_no{ p_config.get_level_tileset(m_current_level) };

	// draw background
	for (int j{ 0 }; j < c::LEVEL_H; ++j)
		for (int i{ 0 }; i < c::LEVEL_W; ++i) {
			int l_tile_no{ MD_BYTE_NO_EMPTY_TILE };
			auto l_ttype = m_levels.at(m_current_level).get_wall_type(i, j);
			if (l_ttype == skc::Wall::Brown)
				l_tile_no = MD_BYTE_NO_BLOCK_BROWN;
			else if (l_ttype == skc::Wall::White || l_ttype == skc::Wall::Brown_white)
				l_tile_no = MD_BYTE_NO_BLOCK_WHITE;

			klib::gfx::blit_scale(p_rnd,
				m_gfx.get_meta_tile(l_tile_no, l_tileset_no),
				20 + i * TILE_SIZE_VISUAL, 20 + j * TILE_SIZE_VISUAL, TILE_SIZE_VISUAL, TILE_SIZE_VISUAL);
		}

	// draw door
	auto l_door_pos = m_levels.at(m_current_level).get_door_pos();
	if (l_door_pos.second >= 0)
		klib::gfx::blit_scale(p_rnd,
			m_gfx.get_meta_tile(MD_BYTE_NO_DOOR, l_tileset_no),
			20 + l_door_pos.first * TILE_SIZE_VISUAL,
			20 + l_door_pos.second * TILE_SIZE_VISUAL, TILE_SIZE_VISUAL, TILE_SIZE_VISUAL);

	// draw player start
	auto l_pstart = m_levels.at(m_current_level).get_player_start_pos();
	klib::gfx::blit_scale(p_rnd,
		m_gfx.get_meta_tile(MD_BYTE_NO_PLAYER_START, l_tileset_no),
		20 + l_pstart.first * TILE_SIZE_VISUAL,
		20 + l_pstart.second * TILE_SIZE_VISUAL, TILE_SIZE_VISUAL, TILE_SIZE_VISUAL);

	// draw key
	auto l_key_pos = m_levels.at(m_current_level).get_key_pos();
	if (l_key_pos.second >= 0)
		klib::gfx::blit_scale(p_rnd,
			m_gfx.get_meta_tile(MD_BYTE_NO_KEY, l_tileset_no),
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
			m_gfx.get_meta_tile(m_levels.at(m_current_level).get_constellation_no(), l_tileset_no),
			20 + l_pos.first * TILE_SIZE_VISUAL,
			20 + l_pos.second * TILE_SIZE_VISUAL, 3 * TILE_SIZE_VISUAL, 2 * TILE_SIZE_VISUAL);
	}

	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	std::string l_level_str{ "Level " + std::to_string(m_current_level + 1) + "###lvl" };
	ImGui::Begin(l_level_str.c_str());

	ImGui::End();

	ImGui::Render();
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
}

void skc::SKC_Main_window::save_nes_file(const std::string& p_file_path, const SKC_Config& p_config) const {
	std::vector<byte> l_output{ p_config.get_rom_data() };
	std::vector<std::size_t> l_item_offsets, l_enemy_offsets;
	std::size_t l_item_offset{ 0 }, lenemy_offset{ 0 };

	std::vector<byte> l_item_data, l_enemy_data;
	for (std::size_t i{ 0 }; i < m_levels.size(); ++i) {
		auto l_data = m_levels[i].get_item_bytes();
		l_item_data.insert(end(l_item_data), begin(l_data), end(l_data));
		l_item_offsets.push_back(l_item_offset);
		l_item_offset += l_data.size();

		l_data = m_levels[i].get_enemy_bytes();
		l_enemy_data.insert(end(l_enemy_data), begin(l_data), end(l_data));
		l_enemy_offsets.push_back(lenemy_offset);
		lenemy_offset += l_data.size();
	}

	// patch item table
	for (std::size_t i{ 0 }; i < l_item_offsets.size(); ++i) {
		std::size_t l_ram_address = p_config.get_ram_address_from_rom(
			p_config.get_offset_item_data() + l_item_offsets[i]);

		byte l_hi = static_cast<byte>(l_ram_address / 256);
		byte l_lo = static_cast<byte>(l_ram_address % 256);

		l_output.at(p_config.get_offset_item_table_hi() + i) = l_hi;
		l_output.at(p_config.get_offset_item_table_lo() + i) = l_lo;
	}

	// patch enemy table
	for (std::size_t i{ 0 }; i < l_enemy_offsets.size(); ++i) {
		std::size_t l_ram_address = p_config.get_ram_address_from_rom(
			p_config.get_offset_enemy_data() + l_enemy_offsets[i]);

		byte l_hi = static_cast<byte>(l_ram_address / 256);
		byte l_lo = static_cast<byte>(l_ram_address % 256);

		l_output.at(p_config.get_offset_enemy_table_hi() + i) = l_hi;
		l_output.at(p_config.get_offset_enemy_table_lo() + i) = l_lo;
	}

	// patch item data
	for (std::size_t i{ 0 }; i < l_item_data.size(); ++i)
		l_output.at(p_config.get_offset_item_data() + i) = l_item_data[i];
	// patch enemy data
	for (std::size_t i{ 0 }; i < l_enemy_data.size(); ++i)
		l_output.at(p_config.get_offset_enemy_data() + i) = l_enemy_data[i];

	klib::file::write_bytes_to_file(l_output, p_file_path);
}
