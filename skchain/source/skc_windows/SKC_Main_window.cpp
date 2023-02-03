#include "SKC_Main_window.h"
#include "./../SKC_Game_metadata.h"
#include "./../common/klib/klib_gfx.h"
#include "./../common/klib/klib_file.h"
#include "./../common/klib/klib_util.h"
#include "./../common/klib/IPS_Patch.h"
#include "./../skc_util/Xml_helper.h"
#include "./../skc_util/Imgui_helper.h"
#include "./../skc_util/Rom_expander.h"
#include "./../skc_constants/Constants_application.h"
#include "./../skc_constants/Constants_level.h"
#include "./../skc_constants/Constants_color.h"
#include "./../common/imgui/imgui.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"
#include <algorithm>
#include <map>
#include <utility>
#include <vector>

skc::SKC_Main_window::SKC_Main_window(SDL_Renderer* p_rnd, SKC_Config& p_config) :
	m_gfx{ p_rnd, p_config }, m_current_level{ 0 }, m_selected_type{ 0 }, m_sel_es_index{ 0 },
	m_texture{ SDL_CreateTexture(p_rnd, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, c::LEVEL_W * c::TILE_GFX_SIZE, c::LEVEL_H * c::TILE_GFX_SIZE) },
	m_timer(255, 1, true), m_render_toggles(std::vector<bool>(4, true))
{
	const auto& lr_rom_data{ p_config.get_rom_data() };

	for (byte i{ 0 }; i < 3; ++i)
		m_selected_picker_tile.push_back(p_config.get_tile_picker(i).at(0).second.front());
	m_levels = std::vector<skc::Level>(p_config.get_level_count(), skc::Level());

	if (lr_rom_data.size() == c::ROM_FILE_SIZE) {

		// extract level data
		for (std::size_t i{ 0 }; i < m_levels.size(); ++i) {
			m_levels.at(i).load_block_data(lr_rom_data, p_config.get_offset_block_data(i));
			m_levels.at(i).load_item_data(lr_rom_data, p_config.get_offset_item_data(i));
			m_levels.at(i).load_enemy_data(lr_rom_data, p_config.get_offset_enemy_data(i));
		}

		// extract drop rate metadata
		for (std::size_t i{ 0 }; i < p_config.get_mirror_rate_count(); ++i)
			m_drop_schedules.push_back(
				klib::util::bytes_to_bitmask_1d(lr_rom_data,
					64,
					p_config.get_offset_mirror_rate_data(i))
			);

		// extract drop enemies metadata
		for (std::size_t i{ 0 }; i < p_config.get_mirror_enemy_count(); ++i) {
			std::vector<byte> l_drop_enemy;
			std::size_t emem_offset{ p_config.get_offset_mirror_enemy_data(i) };
			for (std::size_t i{ 0 }; lr_rom_data.at(emem_offset + i) != c::MIRROR_ENEMY_SET_DELIMITER; ++i)
				l_drop_enemy.push_back(lr_rom_data.at(emem_offset + i));
			m_drop_enemies.push_back(l_drop_enemy);
		}
	}
	else {

		// extract level data for expanded ROM
		for (std::size_t i{ 0 }; i < m_levels.size(); ++i) {
			m_levels.at(i) = skc::m66::parse_level(lr_rom_data, i);
			m_levels.at(i).set_spawn_schedule(0, static_cast<byte>(2 * i));
			m_levels.at(i).set_spawn_enemies(0, static_cast<byte>(2 * i));
			m_levels.at(i).set_spawn_schedule(1, static_cast<byte>(2 * i + 1));
			m_levels.at(i).set_spawn_enemies(1, static_cast<byte>(2 * i + 1));
		}
		// extract drop rate metadata
		m_drop_schedules = skc::m66::parse_mirror_drop_schedules(lr_rom_data);
		// extract drop enemies metadata
		m_drop_enemies = skc::m66::parse_mirror_enemy_sets(lr_rom_data);
	}

	m_board_selection = std::vector<std::vector<int>>(
		p_config.get_level_count(), std::vector<int>(3, 0)
		);

	for (const auto& kv : p_config.get_item_bitmasks()) {
		std::size_t l_level_no = kv.first;
		auto l_bitmask = klib::util::bytes_to_bitmask(lr_rom_data, c::LEVEL_W, c::LEVEL_H, kv.second.second);
		byte l_item_no = kv.second.first;

		for (std::size_t j{ 0 }; j < c::LEVEL_H; ++j)
			for (std::size_t i{ 0 }; i < c::LEVEL_W; ++i)
				if (l_bitmask[j][i])
					m_levels.at(l_level_no).add_item(l_item_no, std::make_pair(static_cast<int>(i), static_cast<int>(j)));
	}

	for (std::size_t i{ 0 }; i < p_config.get_meta_tile_count(); ++i) {
		std::size_t l_level_no{ p_config.get_meta_tile_level_no(i) };
		std::size_t l_rom_offset{ p_config.get_meta_tile_rom_offset(i) };

		position l_pos{ l_rom_offset == 0 ?
			p_config.get_meta_tile_position(i) :
			skc::Level::get_position_from_byte(lr_rom_data.at(p_config.get_meta_tile_rom_offset(i))) };
		m_meta_tiles[l_level_no].push_back(std::make_pair(i, l_pos));
	}

	// turn off gridlines by default
	m_render_toggles[c::TOGGLE_GRID_IDX] = false;

	p_config.add_message("Executable folder: " + p_config.get_base_path());
	p_config.add_message("Escape: Toggle foreground, G: Toggle gridlines");
	p_config.add_message("Keyboard Buttons 1-3: Draw blocks under cursor");
	p_config.add_message("Tab/Shift+Tab: Cycle through elements of selected type");
	p_config.add_message("Right Click: Insert Element Picker element");
	p_config.add_message("Shift + Left Click: Move selected board element");
	p_config.add_message("Left Click: Select board element");
	p_config.add_message("Read the documentation for efficient usage tips!");
	p_config.add_message("Build Date: " + std::string(__DATE__) + " " + std::string(__TIME__) + " CET");
	p_config.add_message("Homepage: https://github.com/kaimitai/skchain");
	p_config.add_message("Welcome to Solomon's Keychain by Kai E. Froeland", c::MSG_CODE_SUCCESS);

	/*
	DEBUG: Extract binary item bytes per level
	for (std::size_t i{ 0 }; i < m_levels.size() - 1; ++i) {
		std::vector<byte> l_item_bytes =
			std::vector<byte>(begin(lr_rom_data) + p_config.get_offset_item_data(i),
				begin(lr_rom_data) + p_config.get_offset_item_data(i + 1));
		klib::file::write_bytes_to_file(l_item_bytes, "items-" + klib::util::stringnum(i + 1, 2) + ".dat");
	}
	*/
	//auto l_outp = m66::cleanup_skedit_rom(klib::file::read_file_as_bytes("noname.nes"));
	//klib::file::write_bytes_to_file(l_outp, "noname-cleaned.nes");
	/*
	for (std::size_t i{ 0 }; i < lr_rom_data.size() - 245; ++i) {
		if (lr_rom_data[i] == 0x20 && lr_rom_data[i + 245] == 0x9e) {
			throw std::runtime_error("Found");
		}
	}
	*/
}

void skc::SKC_Main_window::move(int p_delta_ms,
	const klib::User_input& p_input,
	skc::SKC_Config& p_config,
	int p_screen_h) {

	m_timer.move(p_delta_ms);

	if (!ImGui::GetIO().WantCaptureMouse) {
		if (p_input.mw_up() && m_current_level < m_levels.size() - 1)
			++m_current_level;
		else if (p_input.mw_down() && m_current_level > 0)
			--m_current_level;
	}

	if (!ImGui::GetIO().WantCaptureKeyboard) {
		bool l_shift = p_input.is_shift_pressed();

		if (p_input.is_ctrl_pressed() && p_input.is_pressed(SDL_SCANCODE_S))
			save_nes_file(p_config, l_shift);
		else if (p_input.is_pressed(SDL_SCANCODE_DELETE) && is_selected_index_valid())
			delete_selected_index();
		else if (p_input.is_pressed(SDL_SCANCODE_G))
			m_render_toggles[c::TOGGLE_GRID_IDX] = !m_render_toggles[c::TOGGLE_GRID_IDX];
		else if (p_input.is_pressed(SDL_SCANCODE_UP) && m_current_level < m_levels.size() - 1)
			++m_current_level;
		else if (p_input.is_pressed(SDL_SCANCODE_DOWN) && m_current_level > 0)
			--m_current_level;
		else if (p_input.is_pressed(SDL_SCANCODE_END))
			m_current_level = m_levels.size() - 1;
		else if (p_input.is_pressed(SDL_SCANCODE_HOME))
			m_current_level = 0;
		else if (p_input.is_pressed(SDL_SCANCODE_TAB)) {
			if (l_shift)
				decrease_selected_index();
			else
				increase_selected_index();
		}
		else if (p_input.is_pressed(SDL_SCANCODE_ESCAPE))
			this->toggle_render_all();
	}

}

void skc::SKC_Main_window::right_click(const std::pair<int, int>& p_tile_pos, const skc::SKC_Config& p_config, const klib::User_input& p_input) {
	if (p_input.is_pressed(SDL_SCANCODE_1))
		block_click(skc::Wall::None, p_tile_pos);
	else if (p_input.is_pressed(SDL_SCANCODE_2))
		block_click(skc::Wall::Brown, p_tile_pos);
	else if (p_input.is_pressed(SDL_SCANCODE_3))
		block_click(skc::Wall::White, p_tile_pos);
	else if (m_selected_type == c::ELM_TYPE_METADATA)
		right_click_md(p_tile_pos, p_config);
	else if (m_selected_type == c::ELM_TYPE_ITEM)
		right_click_item(p_tile_pos);
	else
		right_click_enemy(p_tile_pos);
}

void skc::SKC_Main_window::left_click(const std::pair<int, int>& p_tile_pos, const skc::SKC_Config& p_config) {
	static std::vector<std::vector<std::size_t>> ls_priorities{
		{c::ELM_TYPE_METADATA, c::ELM_TYPE_ITEM, c::ELM_TYPE_ENEMY},
		{c::ELM_TYPE_ITEM, c::ELM_TYPE_ENEMY, c::ELM_TYPE_METADATA},
		{c::ELM_TYPE_ENEMY, c::ELM_TYPE_ITEM, c::ELM_TYPE_METADATA}
	};
	const auto& l_priority{ ls_priorities.at(m_selected_type) };

	for (std::size_t i{ 0 }; i < l_priority.size(); ++i) {
		std::size_t l_try_type{ l_priority[i] };

		if ((l_try_type == c::ELM_TYPE_ENEMY && left_click_enemy(p_tile_pos)) ||
			(l_try_type == c::ELM_TYPE_ITEM && left_click_item(p_tile_pos)) ||
			(l_try_type == c::ELM_TYPE_METADATA && left_click_metadata(p_tile_pos)))
			break;
	}
}

void skc::SKC_Main_window::shift_click(const std::pair<int, int>& tile_pos, const skc::SKC_Config& p_config) {
	if (m_selected_type == c::ELM_TYPE_METADATA)
		shift_click_metadata(tile_pos, p_config);
	else if (is_selected_index_valid()) {
		auto& l_level{ get_level() };
		auto l_index{ get_selected_index() };
		if (m_selected_type == c::ELM_TYPE_ENEMY)
			l_level.set_enemy_position(l_index, tile_pos);
		else
			l_level.set_item_position(l_index, tile_pos);
	}
}

void skc::SKC_Main_window::shift_click_metadata(const std::pair<int, int>& tile_pos, const SKC_Config& p_config) {
	set_metadata_tile_position(get_selected_index(), tile_pos, p_config);
}

bool skc::SKC_Main_window::left_click_metadata(const std::pair<int, int>& tile_pos) {
	auto& l_level{ get_level() };
	std::optional<byte> l_new_index;

	if (tile_pos == l_level.get_player_start_pos())
		l_new_index = c::MD_BYTE_NO_PLAYER_START;
	else if (tile_pos == l_level.get_door_pos())
		l_new_index = c::MD_BYTE_NO_DOOR;
	else if (tile_pos == l_level.get_key_pos())
		l_new_index = c::MD_BYTE_NO_KEY;
	else if (tile_pos == l_level.get_spawn_position(0))
		l_new_index = c::MD_BYTE_NO_SPAWN01;
	else if (tile_pos == l_level.get_spawn_position(1))
		l_new_index = c::MD_BYTE_NO_SPAWN02;
	else {
		auto l_iter{ m_meta_tiles.find(m_current_level) };
		if (l_iter != end(m_meta_tiles)) {
			for (std::size_t i{ 0 }; i < l_iter->second.size(); ++i)
				if (l_iter->second[i].second == tile_pos)
					l_new_index = static_cast<int>(i) + static_cast<int>(c::MD_BYTE_NO_META_TILE_MIN);

		}
	}

	if (l_new_index) {
		m_selected_type = c::ELM_TYPE_METADATA;
		set_selected_index(l_new_index.value());
		return true;
	}
	else
		return false;
}

bool skc::SKC_Main_window::left_click_enemy(const std::pair<int, int>& tile_pos) {
	auto& l_level{ get_level() };

	int l_index = l_level.get_enemy_index(tile_pos);
	if (l_index >= 0) {
		m_selected_type = c::ELM_TYPE_ENEMY;
		set_selected_index(l_index);
		return true;
	}

	return false;
}

void skc::SKC_Main_window::set_meta_tile_position(std::size_t p_index, const position& p_pos) {
	m_meta_tiles.at(m_current_level).at(p_index).second = p_pos;
}

void skc::SKC_Main_window::block_click(skc::Wall p_wall_type, const position& p_pos) {
	get_level().set_block(p_wall_type, p_pos);
}

bool skc::SKC_Main_window::left_click_item(const std::pair<int, int>& tile_pos) {
	auto& l_level{ get_level() };

	int l_index = l_level.get_item_index(tile_pos);
	if (l_index >= 0) {
		m_selected_type = c::ELM_TYPE_ITEM;
		set_selected_index(l_index);
		return true;
	}

	return false;
}

void skc::SKC_Main_window::delete_selected_index(void) {
	if (is_selected_index_valid()) {
		auto l_index = get_selected_index();
		auto& l_level{ get_level() };

		if (m_selected_type == c::ELM_TYPE_METADATA) {
			if (l_index == c::MD_BYTE_NO_DOOR)
				l_level.set_door_removed();
			else if (l_index == c::MD_BYTE_NO_KEY)
				l_level.set_key_removed();
		}
		else {
			if (m_selected_type == c::ELM_TYPE_ITEM)
				l_level.delete_item(l_index);
			else if (m_selected_type == c::ELM_TYPE_ENEMY)
				l_level.delete_enemy(l_index);

			if (l_index > 0)
				--m_board_selection[m_current_level][m_selected_type];
		}
	}
}

void skc::SKC_Main_window::right_click_enemy(const std::pair<int, int>& tile_pos) {
	auto& l_level{ get_level() };

	if (!l_level.has_enemy_at_position(tile_pos)) {
		l_level.add_enemy(get_tile_selection(), tile_pos);
	}
}

void skc::SKC_Main_window::right_click_item(const std::pair<int, int>& tile_pos) {
	auto& l_level{ get_level() };

	if (!l_level.has_item_at_position(tile_pos)) {
		l_level.add_item(get_tile_selection(), tile_pos);
	}
}

void skc::SKC_Main_window::right_click_md(const std::pair<int, int>& tile_pos, const skc::SKC_Config& p_config) {
	set_metadata_tile_position(get_tile_selection(), tile_pos, p_config);
}

void skc::SKC_Main_window::reset_selections(std::size_t p_level_no) {
	m_board_selection.at(p_level_no) = std::vector<int>(3, 0);
}

std::pair<int, int> skc::SKC_Main_window::pixel_to_tile_pos(int p_screen_h, int p_x, int p_y) const {
	int l_tile_w = get_tile_w(p_screen_h);
	return std::make_pair(p_x / l_tile_w, p_y / l_tile_w);
}

void skc::SKC_Main_window::draw_tile(SDL_Renderer* p_rnd, SDL_Texture* p_texture, int p_x, int p_y, bool p_transp) const {
	if (p_transp) {
		SDL_SetTextureBlendMode(p_texture, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(p_texture, 120);
	}

	klib::gfx::blit_scale(p_rnd,
		p_texture,
		p_x * c::TILE_GFX_SIZE, p_y * c::TILE_GFX_SIZE, c::TILE_SCALE);

	if (p_transp)
		SDL_SetTextureAlphaMod(p_texture, 255);
}

void skc::SKC_Main_window::generate_texture(SDL_Renderer* p_rnd, const SKC_Config& p_config) {
	SDL_SetRenderTarget(p_rnd, m_texture);

	const auto& l_level{ get_level() };
	std::size_t l_tileset_no{ p_config.get_level_tileset(m_current_level,
		l_level.get_tileset_no()) };

	// draw empty background
	for (int j{ 0 }; j < c::LEVEL_H; ++j)
		for (int i{ 0 }; i < c::LEVEL_W; ++i)
			draw_tile(p_rnd, m_gfx.get_meta_tile(c::MD_BYTE_NO_EMPTY_TILE, l_tileset_no), i, j);

	// draw constellation
	if (l_level.has_constellation()) {
		auto l_pos = l_level.get_constellation_pos();
		draw_tile(p_rnd, m_gfx.get_meta_tile(l_level.get_constellation_no(), l_tileset_no),
			l_pos.first, l_pos.second);
	}

	// expanded rom draw order: mirrors, blocks, items, door, key, meta tiles
	if (is_rom_expanded()) {
		if (m_render_toggles[c::TOGGLE_META_IDX])
			generate_texture_mirrors(p_rnd, p_config, l_level, l_tileset_no);
		generate_texture_blocks(p_rnd, p_config, l_level, l_tileset_no);
		if (m_render_toggles[c::TOGGLE_ITEM_IDX])
			generate_texture_items(p_rnd, p_config, l_level, l_tileset_no);
		if (m_render_toggles[c::TOGGLE_META_IDX]) {
			generate_texture_door_and_key(p_rnd, p_config, l_level, l_tileset_no);
		}
	}
	// regular rom draw order: blocks, door, key, mirrors, items, meta tiles
	else {
		generate_texture_blocks(p_rnd, p_config, l_level, l_tileset_no);
		if (m_render_toggles[c::TOGGLE_META_IDX]) {
			generate_texture_door_and_key(p_rnd, p_config, l_level, l_tileset_no);
			generate_texture_mirrors(p_rnd, p_config, l_level, l_tileset_no);
		}
		if (m_render_toggles[c::TOGGLE_ITEM_IDX])
			generate_texture_items(p_rnd, p_config, l_level, l_tileset_no);
	}

	if (m_render_toggles[c::TOGGLE_META_IDX])
		generate_texture_meta_tiles(p_rnd, p_config, l_level, l_tileset_no);

	// draw enemies
	if (m_render_toggles[c::TOGGLE_ENEMY_IDX]) {
		const auto& l_enemies = l_level.get_enemies();
		for (const auto& enemy : l_enemies) {
			byte l_no = enemy.get_element_no();
			auto l_pos = enemy.get_position();
			draw_tile(p_rnd, m_gfx.get_enemy_tile(enemy.get_element_no(), l_tileset_no),
				l_pos.first, l_pos.second);
		}
	}

	if (m_render_toggles[c::TOGGLE_META_IDX]) {
		// draw player start
		auto l_pstart = l_level.get_player_start_pos();
		draw_tile(p_rnd, m_gfx.get_meta_tile(l_pstart.first >= c::LEVEL_W / 2 ? c::MD_BYTE_NO_PLAYER_START_LEFT : c::MD_BYTE_NO_PLAYER_START, l_tileset_no),
			l_pstart.first, l_pstart.second);
	}

	// draw gridlines
	if (m_render_toggles[c::TOGGLE_GRID_IDX]) {
		auto l_pulse_color{ klib::gfx::pulse_color(c::COL_WHITE, c::COL_GOLD, m_timer.get_frame()) };

		SDL_SetRenderDrawColor(p_rnd, l_pulse_color.r,
			l_pulse_color.g, l_pulse_color.b, 0);

		for (int i{ 1 }; i < c::LEVEL_W; ++i)
			SDL_RenderDrawLine(p_rnd, i * c::TILE_GFX_SIZE, 0, i * c::TILE_GFX_SIZE, c::LEVEL_H * c::TILE_GFX_SIZE);
		for (int i{ 1 }; i < c::LEVEL_H; ++i)
			SDL_RenderDrawLine(p_rnd, 0, i * c::TILE_GFX_SIZE, c::LEVEL_W * c::TILE_GFX_SIZE, i * c::TILE_GFX_SIZE);
	}

	if (is_selected_index_valid()) {
		std::pair<int, int> l_pos{ 0,0 };
		int l_board_index{ get_selected_index() };
		if (m_selected_type == c::ELM_TYPE_ITEM)
			l_pos = l_level.get_items().at(l_board_index).get_position();
		else if (m_selected_type == c::ELM_TYPE_ENEMY)
			l_pos = l_level.get_enemies().at(l_board_index).get_position();
		else
			l_pos = get_metadata_tile_position(l_board_index);

		auto l_pulse_color{ klib::gfx::pulse_color(c::COL_YELLOW, c::COL_ORANGE, m_timer.get_frame()) };
		klib::gfx::draw_rect(p_rnd,
			l_pos.first * c::TILE_GFX_SIZE, l_pos.second * c::TILE_GFX_SIZE,
			c::TILE_GFX_SIZE, c::TILE_GFX_SIZE, l_pulse_color, 2);
	}

	SDL_SetRenderTarget(p_rnd, nullptr);
}

void skc::SKC_Main_window::generate_texture_mirrors(SDL_Renderer* p_rnd,
	const SKC_Config& p_config, const skc::Level& p_level, std::size_t p_tileset_no) {
	auto l_m1_pos{ p_level.get_spawn_position(0) };
	auto l_m2_pos{ p_level.get_spawn_position(1) };
	draw_tile(p_rnd, m_gfx.get_meta_tile(c::MD_BYTE_NO_SPAWN01, p_tileset_no), l_m1_pos.first, l_m1_pos.second);
	draw_tile(p_rnd, m_gfx.get_meta_tile(c::MD_BYTE_NO_SPAWN02, p_tileset_no), l_m2_pos.first, l_m2_pos.second);
}

void skc::SKC_Main_window::generate_texture_blocks(SDL_Renderer* p_rnd, const SKC_Config& p_config,
	const skc::Level& p_level, std::size_t p_tileset_no) {
	for (int j{ 0 }; j < c::LEVEL_H; ++j)
		for (int i{ 0 }; i < c::LEVEL_W; ++i) {
			byte l_tile_no{ 0 };
			auto l_ttype = p_level.get_wall_type(i, j);
			if (l_ttype == skc::Wall::Brown)
				l_tile_no = c::MD_BYTE_NO_BLOCK_BROWN;
			else if (l_ttype == skc::Wall::White || l_ttype == skc::Wall::Brown_white)
				l_tile_no = c::MD_BYTE_NO_BLOCK_WHITE;

			if (l_tile_no != 0)
				draw_tile(p_rnd, m_gfx.get_meta_tile(l_tile_no, p_tileset_no), i, j);
		}
}

void skc::SKC_Main_window::generate_texture_door_and_key(SDL_Renderer* p_rnd, const SKC_Config& p_config,
	const skc::Level& p_level, std::size_t p_tileset_no) {

	// draw door
	auto l_door_pos = p_level.get_door_pos();
	if (l_door_pos.second >= 0)
		draw_tile(p_rnd, m_gfx.get_meta_tile(m_current_level == 49 ? c::MD_BYTE_NO_SOLOMONS_KEY : c::MD_BYTE_NO_DOOR, p_tileset_no),
			l_door_pos.first, l_door_pos.second);

	// draw key
	if (!p_level.is_key_removed()) {
		auto l_key_pos = m_levels.at(m_current_level).get_key_pos();

		draw_tile(p_rnd, m_gfx.get_meta_tile(c::MD_BYTE_NO_KEY, p_tileset_no),
			l_key_pos.first, l_key_pos.second, p_level.is_key_hidden());
		if (p_level.is_key_in_block()) {
			draw_tile(p_rnd, m_gfx.get_meta_tile(c::MD_BYTE_NO_BLOCK_BROWN,
				p_tileset_no), l_key_pos.first, l_key_pos.second, true);
		}
	}
}

void skc::SKC_Main_window::generate_texture_meta_tiles(SDL_Renderer* p_rnd, const SKC_Config& p_config,
	const skc::Level& p_level, std::size_t p_tileset_no) {

	// draw meta-items
	auto iter = m_meta_tiles.find(m_current_level);
	if (iter != end(m_meta_tiles))
		for (const auto& l_mtile : iter->second) {
			std::size_t l_index{ l_mtile.first };
			auto l_pos{ l_mtile.second };
			draw_tile(p_rnd, m_gfx.get_absolute_tile(p_config.get_meta_tile_tile_no(l_index), p_tileset_no),
				l_pos.first, l_pos.second, p_config.get_meta_tile_transparent(l_index));
		}

}

void skc::SKC_Main_window::generate_texture_items(SDL_Renderer* p_rnd, const SKC_Config& p_config,
	const skc::Level& p_level, std::size_t p_tileset_no) {
	// draw items
	const auto& l_items = p_level.get_items();
	for (const auto& item : l_items) {
		byte l_no = item.get_item_no();
		auto l_pos = item.get_position();
		draw_tile(p_rnd, m_gfx.get_item_tile(item.get_item_no(), p_tileset_no),
			l_pos.first, l_pos.second,
			skc::Level::is_item_hidden(item.get_element_no()));
		if (skc::Level::is_item_in_block(item.get_element_no()))
			draw_tile(p_rnd, m_gfx.get_meta_tile(c::MD_BYTE_NO_BLOCK_BROWN,
				p_tileset_no), l_pos.first, l_pos.second, true);
	}
}

int skc::SKC_Main_window::get_tile_w(int p_screen_h) const {
	return std::max<int>(1, p_screen_h / c::LEVEL_H);
}

void skc::SKC_Main_window::draw(SDL_Renderer* p_rnd, SKC_Config& p_config,
	const klib::User_input& p_input, int p_w, int p_h) {
	this->generate_texture(p_rnd, p_config);

	SDL_SetRenderDrawColor(p_rnd, 126, 126, 255, 0);
	SDL_RenderClear(p_rnd);

	this->draw_ui(p_config, p_input);
}

std::vector<byte> skc::SKC_Main_window::generate_patch_bytes(SKC_Config& p_config) const {
	if (p_config.get_rom_data().size() == c::ROM_M66_FILE_SIZE ||
		m66::is_rom_expanded(m_drop_enemies.size()))
		return generate_patch_bytes_rom66(p_config);
	else
		return generate_patch_bytes_rom03(p_config);

}

// generate regular ROM ("vanilla" hack)
std::vector<byte> skc::SKC_Main_window::generate_patch_bytes_rom03(SKC_Config& p_config) const {
	std::vector<byte> l_output{ p_config.get_rom_data() };
	std::vector<std::size_t> l_item_offsets, l_enemy_offsets, l_enemy_sets_offsets;
	std::size_t l_item_offset{ 0 }, lenemy_offset{ 0 };
	const auto& l_item_bitmasks{ p_config.get_item_bitmasks() };

	const auto check_data_section_size = [&p_config](const std::string& p_section_name,
		std::size_t p_actual_value, std::size_t p_max_value) -> void {
			int l_msg_color{ c::MSG_CODE_INFO };
			if (p_actual_value > p_max_value)
				l_msg_color = c::MSG_CODE_ERROR;
			else if (p_actual_value == p_max_value)
				l_msg_color = c::MSG_CODE_WARNING;

			p_config.add_message(p_section_name + " data size (bytes): " +
				std::to_string(p_actual_value) + "/" + std::to_string(p_max_value),
				l_msg_color);
			if (p_actual_value > p_max_value)
				throw std::runtime_error(p_section_name + " data section too big");
	};

	std::vector<byte> l_item_data, l_enemy_data, l_block_data;
	for (std::size_t i{ 0 }; i < m_levels.size(); ++i) {

		std::vector<byte> l_ignore_item_element_nos;
		if (l_item_bitmasks.find(i) != end(l_item_bitmasks))
			l_ignore_item_element_nos.push_back(l_item_bitmasks.at(i).first);

		auto l_data = m_levels[i].get_item_bytes(l_ignore_item_element_nos);
		l_item_data.insert(end(l_item_data), begin(l_data), end(l_data));
		l_item_offsets.push_back(l_item_offset);
		l_item_offset += l_data.size();

		l_data = m_levels[i].get_enemy_bytes();
		l_enemy_data.insert(end(l_enemy_data), begin(l_data), end(l_data));
		l_enemy_offsets.push_back(lenemy_offset);
		lenemy_offset += l_data.size();

		l_data = m_levels[i].get_block_bytes();
		l_block_data.insert(end(l_block_data), begin(l_data), end(l_data));
	}

	// patch block data
	for (std::size_t i{ 0 }; i < l_block_data.size(); ++i)
		l_output.at(p_config.get_offset_block_data() + i) = l_block_data[i];

	// patch item table
	for (std::size_t i{ 0 }; i < l_item_offsets.size(); ++i) {
		auto l_ram_address = p_config.get_ram_address_bytes_from_rom(
			p_config.get_offset_item_data() + l_item_offsets[i]);

		l_output.at(p_config.get_offset_item_table_hi() + i) = l_ram_address.first;
		l_output.at(p_config.get_offset_item_table_lo() + i) = l_ram_address.second;
	}

	// patch enemy table
	for (std::size_t i{ 0 }; i < l_enemy_offsets.size(); ++i) {
		auto l_ram_address = p_config.get_ram_address_bytes_from_rom(
			p_config.get_offset_enemy_data() + l_enemy_offsets[i]);

		l_output.at(p_config.get_offset_enemy_table_hi() + i) = l_ram_address.first;
		l_output.at(p_config.get_offset_enemy_table_lo() + i) = l_ram_address.second;
	}

	// apply signature
	std::string l_signature{ c::EDITOR_SIGNATURE };
	if (l_item_data.size() + l_signature.size() <= p_config.get_length_item_data()) {
		l_item_data.insert(end(l_item_data), begin(l_signature), end(l_signature));
	}

	// patch item data
	for (std::size_t i{ 0 }; i < l_item_data.size(); ++i)
		l_output.at(p_config.get_offset_item_data() + i) = l_item_data[i];
	// patch enemy data
	for (std::size_t i{ 0 }; i < l_enemy_data.size(); ++i)
		l_output.at(p_config.get_offset_enemy_data() + i) = l_enemy_data[i];

	// patch item bitmasks
	for (const auto& kv : l_item_bitmasks) {
		byte l_item_element_no{ kv.second.first };
		std::size_t l_offset{ kv.second.second };
		auto l_bitmask_bytes{ m_levels.at(kv.first).get_item_bitmask_bytes(l_item_element_no) };

		klib::util::append_or_overwrite_vector(l_output, l_bitmask_bytes, l_offset);
	}

	// patch meta-tiles
	for (const auto& kv : m_meta_tiles)
		for (const auto& l_md_tile : kv.second) {
			std::size_t l_md_index{ l_md_tile.first };
			if (p_config.get_meta_tile_movable(l_md_index)) {
				std::size_t l_offset{ p_config.get_meta_tile_rom_offset(l_md_index) };
				l_output.at(l_offset) = skc::Level::get_byte_from_position(l_md_tile.second);
			}
		}

	// patch drop schedules
	std::vector<byte> l_drop_data;

	for (std::size_t i{ 0 }; i < p_config.get_mirror_rate_count(); ++i) {
		std::vector<byte> l_drop_scehd{ klib::util::bitmask_to_bytes(m_drop_schedules[i]) };
		l_drop_data.insert(end(l_drop_data), begin(l_drop_scehd), end(l_drop_scehd));
	}


	klib::util::append_or_overwrite_vector(l_output, l_drop_data, p_config.get_offset_mirror_rate_data(0));

	// patch drop enemy sets table and drop enemy sets
	std::vector<byte> l_enemy_sets_data;
	for (std::size_t i{ 0 }; i < p_config.get_mirror_enemy_count(); ++i) {
		l_enemy_sets_offsets.push_back(l_enemy_sets_data.size());
		l_enemy_sets_data.insert(end(l_enemy_sets_data), begin(m_drop_enemies.at(i)), end(m_drop_enemies.at(i)));
		l_enemy_sets_data.push_back(c::MIRROR_ENEMY_SET_DELIMITER);
	}

	for (std::size_t i{ 0 }; i < l_enemy_sets_offsets.size(); ++i) {
		auto l_ram_address{ p_config.get_ram_address_bytes_from_rom(
			p_config.get_offset_mirror_enemy_data(0) + l_enemy_sets_offsets[i]) };

		l_output.at(p_config.get_offset_mirror_enemy_table_hi() + i) = l_ram_address.first;
		l_output.at(p_config.get_offset_mirror_enemy_table_lo() + i) = l_ram_address.second;
	}

	klib::util::append_or_overwrite_vector(l_output, l_enemy_sets_data,
		p_config.get_offset_mirror_enemy_data(0));

	check_data_section_size("Drop Schedule", l_drop_data.size(), p_config.get_length_mirror_rate_data());
	check_data_section_size("Enemy Set", l_enemy_sets_data.size(), p_config.get_length_mirror_enemy_data());
	check_data_section_size("Enemy", l_enemy_data.size(), p_config.get_length_enemy_data());
	check_data_section_size("Item", l_item_data.size(), p_config.get_length_item_data());

	return l_output;
}

// generate expanded ROM
std::vector<byte> skc::SKC_Main_window::generate_patch_bytes_rom66(SKC_Config& p_config) const {
	std::vector<byte> l_output{ p_config.get_rom_data() };

	if (p_config.get_region_code() == c::REGION_US)
		l_output = m66::change_mapper(l_output);

	skc::m66::patch_mirror_drop_schedule_bytes(l_output, m_drop_schedules, m_levels);
	skc::m66::patch_mirror_enemy_set_bytes(l_output, m_drop_enemies, m_levels);
	skc::m66::patch_enemy_data_bytes(l_output, m_levels);
	skc::m66::patch_item_data_bytes(l_output, m_levels);

	// patch meta-tiles
	for (const auto& kv : m_meta_tiles)
		for (const auto& l_md_tile : kv.second) {
			std::size_t l_md_index{ l_md_tile.first };
			if (p_config.get_meta_tile_movable(l_md_index)) {
				std::size_t l_offset{ p_config.get_meta_tile_rom_offset(l_md_index) };
				l_output.at(l_offset) = skc::Level::get_byte_from_position(l_md_tile.second);
			}
		}

	l_output = skc::m66::cleanup_skedit_rom(l_output);

	return l_output;
}

bool skc::SKC_Main_window::is_valid_constellation(byte p_constellation) const {
	byte l_tileset_no = get_level().get_tileset_no();
	byte l_constellation = p_constellation - c::ITEM_CONSTELLATION_ARIES;

	return l_constellation >= l_tileset_no * 4 &&
		l_constellation < (l_tileset_no + 1) * 4;
}

const skc::Level& skc::SKC_Main_window::get_level(void) const {
	return m_levels.at(m_current_level);
}

skc::Level& skc::SKC_Main_window::get_level(void) {
	return m_levels.at(m_current_level);
}

int skc::SKC_Main_window::get_selected_index(void) const {
	return m_board_selection[m_current_level][m_selected_type];
}

void skc::SKC_Main_window::set_selected_index(int p_index) {
	m_board_selection[m_current_level][m_selected_type] = p_index;
}

bool skc::SKC_Main_window::is_selected_index_valid(void) const {
	int l_index{ get_selected_index() };

	const auto& l_level{ get_level() };

	return (m_selected_type == c::ELM_TYPE_ITEM &&
		l_index < static_cast<int>(l_level.get_items().size())) ||
		(m_selected_type == c::ELM_TYPE_ENEMY &&
			l_index < static_cast<int>(l_level.get_enemies().size())) ||
		m_selected_type == c::ELM_TYPE_METADATA;
}

void skc::SKC_Main_window::increase_selected_index(void) {
	int l_index_count{ get_selected_index_count() };
	int l_index{ get_selected_index() };

	if (l_index_count == 0)
		return;

	if (m_selected_type == c::ELM_TYPE_METADATA
		&& l_index == c::MD_BYTE_NO_SPAWN02) {
		int l_count = get_selected_index_count();
		if (l_count == c::MD_BYTE_NO_SPAWN02 + 1)
			set_selected_index(0);
		else
			set_selected_index(c::MD_BYTE_NO_META_TILE_MIN);
		return;
	}

	set_selected_index(l_index + 1);
	if (l_index + 1 >= get_selected_index_count())
		set_selected_index(0);
}

void skc::SKC_Main_window::decrease_selected_index(void) {
	int l_index_count{ get_selected_index_count() };
	int l_index{ get_selected_index() };

	if (l_index_count == 0)
		return;

	if (m_selected_type == c::ELM_TYPE_METADATA &&
		l_index == c::MD_BYTE_NO_META_TILE_MIN) {
		set_selected_index(c::MD_BYTE_NO_SPAWN02);
		return;
	}

	set_selected_index(l_index - 1);
	if (l_index == 0)
		set_selected_index(l_index_count - 1);
}

int skc::SKC_Main_window::get_selected_index_count(void) const {
	int l_index{ get_selected_index() };
	const auto& l_level{ get_level() };

	if (m_selected_type == c::ELM_TYPE_ITEM)
		return l_level.get_item_count();
	else if (m_selected_type == c::ELM_TYPE_ENEMY)
		return l_level.get_enemy_count();
	else {
		int l_mt_count{ 0 };
		auto iter = m_meta_tiles.find(m_current_level);
		if (iter != end(m_meta_tiles)) {
			l_mt_count = static_cast<int>(iter->second.size());
			return c::MD_BYTE_NO_META_TILE_MIN + l_mt_count;
		}
		else
			return c::MD_BYTE_NO_SPAWN02 + 1;
	}
}

byte skc::SKC_Main_window::get_tile_selection(void) const {
	return m_selected_picker_tile[m_selected_type];
}

void skc::SKC_Main_window::set_tile_selection(byte p_value) {
	m_selected_picker_tile[m_selected_type] = p_value;
}

position skc::SKC_Main_window::get_metadata_tile_position(byte p_board_index) const {
	auto& l_level = get_level();

	if (p_board_index == c::MD_BYTE_NO_DOOR)
		return l_level.get_door_pos();
	else if (p_board_index == c::MD_BYTE_NO_KEY)
		return l_level.get_key_pos();
	else if (p_board_index == c::MD_BYTE_NO_PLAYER_START)
		return l_level.get_player_start_pos();
	else if (p_board_index == c::MD_BYTE_NO_SPAWN01)
		return l_level.get_spawn_position(0);
	else if (p_board_index == c::MD_BYTE_NO_CONSTELLATION)
		return l_level.get_constellation_pos();
	else if (p_board_index == c::MD_BYTE_NO_SPAWN02)
		return l_level.get_spawn_position(1);
	else if (p_board_index == c::MD_BYTE_NO_CONSTELLATION && l_level.has_constellation())
		return l_level.get_constellation_pos();
	else if (p_board_index >= c::MD_BYTE_NO_META_TILE_MIN)
		return m_meta_tiles.at(m_current_level).at(p_board_index - c::MD_BYTE_NO_META_TILE_MIN).second;
	else
		throw std::runtime_error("Invalid index");
}

void skc::SKC_Main_window::set_metadata_tile_position(byte p_board_index_no, const position& p_pos, const SKC_Config& p_config) {
	auto& l_level{ get_level() };

	if (p_board_index_no == c::MD_BYTE_NO_BLOCK_BROWN)
		l_level.set_block(skc::Wall::Brown, p_pos);
	else if (p_board_index_no == c::MD_BYTE_NO_EMPTY_TILE)
		l_level.set_block(skc::Wall::None, p_pos);
	else if (p_board_index_no == c::MD_BYTE_NO_BLOCK_WHITE)
		l_level.set_block(skc::Wall::White, p_pos);
	else if (p_board_index_no == c::MD_BYTE_NO_BLOCK_BW)
		l_level.set_block(skc::Wall::Brown_white, p_pos);
	else if (p_board_index_no == c::MD_BYTE_NO_PLAYER_START)
		l_level.set_player_start_pos(p_pos);
	else if (p_board_index_no == c::MD_BYTE_NO_KEY)
		l_level.set_key_pos(p_pos);
	else if (p_board_index_no == c::MD_BYTE_NO_DOOR)
		l_level.set_door_pos(p_pos);
	else if (p_board_index_no == c::MD_BYTE_NO_SPAWN01)
		l_level.set_spawn_position(0, p_pos);
	else if (p_board_index_no == c::MD_BYTE_NO_SPAWN02)
		l_level.set_spawn_position(1, p_pos);
	else if (p_board_index_no >= c::ITEM_CONSTELLATION_ARIES)
		l_level.set_constellation(p_board_index_no, p_pos);
	else {
		std::size_t l_selected_meta_index = static_cast<std::size_t>(p_board_index_no - c::MD_BYTE_NO_META_TILE_MIN);
		auto iter = m_meta_tiles.find(m_current_level);
		if (iter != end(m_meta_tiles) && l_selected_meta_index < iter->second.size() &&
			p_config.get_meta_tile_movable(iter->second.at(l_selected_meta_index).first)) {
			set_meta_tile_position(l_selected_meta_index, p_pos);
		}
	}
}

void skc::SKC_Main_window::toggle_render_all(void) {
	bool l_new_value = !m_render_toggles[c::TOGGLE_META_IDX];

	for (std::size_t i{ c::TOGGLE_GRID_IDX + 1 }; i <= c::TOGGLE_ENEMY_IDX; ++i)
		m_render_toggles[i] = l_new_value;
}

bool skc::SKC_Main_window::is_rom_expanded(void) const {
	return m66::is_rom_expanded(m_drop_schedules.size());
}

void skc::SKC_Main_window::set_application_icon(SDL_Window* p_window) const {
	m_gfx.set_application_icon(p_window);
}

// procedure that will change the current level data into an expanded version of the same data
void skc::SKC_Main_window::expand_rom_data(SKC_Config& p_config) {
	m_drop_enemies = m66::expand_enemy_sets(m_drop_enemies, m_levels);
	m_drop_schedules = m66::expand_drop_schedules(m_drop_schedules, m_levels);

	for (std::size_t i{ 0 }; i < m_levels.size(); ++i) {
		m_levels[i].set_spawn_enemies(0, static_cast<byte>(2 * i));
		m_levels[i].set_spawn_enemies(1, static_cast<byte>(2 * i + 1));
		m_levels[i].set_spawn_schedule(0, static_cast<byte>(2 * i));
		m_levels[i].set_spawn_schedule(1, static_cast<byte>(2 * i + 1));
	}

	m66::remove_blocks_behind_demon_mirrors(m_levels);

	p_config.add_message("Expanded ROM size and gave Demon Mirrors separate data", c::MSG_CODE_SUCCESS);
}
