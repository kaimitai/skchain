#include "SKC_Main_window.h"
#include "./../common/klib/klib_gfx.h"
#include "./../common/klib/klib_file.h"
#include "./../skc_util/Xml_helper.h"
#include "./../skc_constants/Constants_level.h"
#include "./../common/imgui/imgui.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"
#include <algorithm>
#include <map>
#include <utility>
#include <vector>

skc::SKC_Main_window::SKC_Main_window(SDL_Renderer* p_rnd, const SKC_Config& p_config) :
	m_gfx{ p_rnd, p_config }, m_current_level{ 0 }, m_selected_type{ 0 }, m_selected{ 0 },
	m_texture{ SDL_CreateTexture(p_rnd, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, c::LEVEL_W * c::TILE_GFX_SIZE, c::LEVEL_H * c::TILE_GFX_SIZE) }
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
		m_levels.at(i).set_tileset_no(p_config.get_level_tileset(i) % 3);
	}

	m_board_selection = std::vector<std::vector<int>>(
		p_config.get_level_count(), std::vector<int>(3, 0)
		);

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

void skc::SKC_Main_window::move(int p_delta_ms,
	const klib::User_input& p_input,
	skc::SKC_Config& p_config,
	int p_screen_h) {
	if (p_input.mw_up() && m_current_level < m_levels.size() - 1)
		++m_current_level;
	else if (p_input.mw_down() && m_current_level > 0)
		--m_current_level;
	else if (p_input.mouse_held(false)) {
		auto l_tile_pos = pixel_to_tile_pos(p_screen_h, p_input.mx(), p_input.my());
		if (l_tile_pos.first < c::LEVEL_W && l_tile_pos.second < c::LEVEL_H)
			right_click(l_tile_pos);
	}
	else if (p_input.mouse_held()) {
		auto l_tile_pos = pixel_to_tile_pos(p_screen_h, p_input.mx(), p_input.my());
		if (l_tile_pos.first < c::LEVEL_W && l_tile_pos.second < c::LEVEL_H)
			left_click(l_tile_pos);
	}

	if (p_input.is_ctrl_pressed() && p_input.is_pressed(SDL_SCANCODE_S))
		save_nes_file("sk_test.nes", p_config);
	else if (p_input.is_pressed(SDL_SCANCODE_DELETE))
		delete_selected_index();
}

void skc::SKC_Main_window::right_click(const std::pair<int, int>& p_tile_pos) {
	if (m_selected_type == c::ELM_TYPE_METADATA)
		right_click_md(p_tile_pos);
	else if (m_selected_type == c::ELM_TYPE_ITEM)
		right_click_item(p_tile_pos);
	else
		right_click_enemy(p_tile_pos);
}

void skc::SKC_Main_window::left_click(const std::pair<int, int>& p_tile_pos) {
	if (m_selected_type == c::ELM_TYPE_ENEMY)
		left_click_enemy(p_tile_pos);
	else if (m_selected_type == c::ELM_TYPE_ITEM)
		left_click_item(p_tile_pos);
}

void skc::SKC_Main_window::left_click_enemy(const std::pair<int, int>& tile_pos) {
	auto& l_level{ get_level() };

	int l_index = l_level.get_enemy_index(tile_pos);
	if (l_index >= 0)
		set_selected_index(l_index);
}

void skc::SKC_Main_window::left_click_item(const std::pair<int, int>& tile_pos) {
	auto& l_level{ get_level() };

	int l_index = l_level.get_item_index(tile_pos);
	if (l_index >= 0)
		set_selected_index(l_index);
}

void skc::SKC_Main_window::delete_selected_index(void) {
	if (is_selected_index_valid()) {
		auto& l_level{ get_level() };
		auto l_index = get_selected_index();

		if (m_selected_type == c::ELM_TYPE_ITEM)
			l_level.delete_item(l_index);
		else if (m_selected_type == c::ELM_TYPE_ENEMY)
			l_level.delete_enemy(l_index);

		if (l_index > 0)
			--m_board_selection[m_current_level][m_selected_type];
	}
}

void skc::SKC_Main_window::right_click_enemy(const std::pair<int, int>& tile_pos) {
	auto& l_level{ get_level() };

	if (!l_level.has_enemy_at_position(tile_pos)) {
		l_level.add_enemy(m_selected, tile_pos);
	}
}

void skc::SKC_Main_window::right_click_item(const std::pair<int, int>& tile_pos) {
	auto& l_level{ get_level() };

	if (!l_level.has_item_at_position(tile_pos)) {
		l_level.add_item(m_selected, tile_pos);
	}
}

void skc::SKC_Main_window::right_click_md(const std::pair<int, int>& tile_pos) {
	auto& l_level{ get_level() };

	if (m_selected == c::MD_BYTE_NO_BLOCK_BROWN)
		l_level.set_block(skc::Wall::Brown, tile_pos);
	else if (m_selected == c::MD_BYTE_NO_EMPTY_TILE)
		l_level.set_block(skc::Wall::None, tile_pos);
	else if (m_selected == c::MD_BYTE_NO_BLOCK_WHITE)
		l_level.set_block(skc::Wall::White, tile_pos);
	else if (m_selected == c::MD_BYTE_NO_BLOCK_BW)
		l_level.set_block(skc::Wall::Brown_white, tile_pos);

	else if (m_selected == c::MD_BYTE_NO_PLAYER_START)
		l_level.set_player_start_pos(tile_pos);
	else if (m_selected == c::MD_BYTE_NO_KEY)
		l_level.set_key_pos(tile_pos);
	else if (m_selected == c::MD_BYTE_NO_DOOR)
		l_level.set_door_pos(tile_pos);
	else if (m_selected >= c::ITEM_CONSTELLATION_ARIES)
		l_level.set_constellation(m_selected, tile_pos);
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

	std::size_t l_tileset_no{ p_config.get_level_tileset(m_current_level) };
	const auto& l_level{ get_level() };

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

	// draw background
	for (int j{ 0 }; j < c::LEVEL_H; ++j)
		for (int i{ 0 }; i < c::LEVEL_W; ++i) {
			byte l_tile_no{ 0 };
			auto l_ttype = l_level.get_wall_type(i, j);
			if (l_ttype == skc::Wall::Brown)
				l_tile_no = c::MD_BYTE_NO_BLOCK_BROWN;
			else if (l_ttype == skc::Wall::White || l_ttype == skc::Wall::Brown_white)
				l_tile_no = c::MD_BYTE_NO_BLOCK_WHITE;

			if (l_tile_no != 0)
				draw_tile(p_rnd, m_gfx.get_meta_tile(l_tile_no, l_tileset_no), i, j);
		}

	// draw door
	auto l_door_pos = l_level.get_door_pos();
	if (l_door_pos.second >= 0)
		draw_tile(p_rnd, m_gfx.get_meta_tile(c::MD_BYTE_NO_DOOR, l_tileset_no),
			l_door_pos.first, l_door_pos.second);

	// draw player start
	auto l_pstart = l_level.get_player_start_pos();
	draw_tile(p_rnd, m_gfx.get_meta_tile(c::MD_BYTE_NO_PLAYER_START, l_tileset_no),
		l_pstart.first, l_pstart.second);

	// draw key
	if (!l_level.is_key_removed()) {
		auto l_key_pos = m_levels.at(m_current_level).get_key_pos();

		draw_tile(p_rnd, m_gfx.get_meta_tile(c::MD_BYTE_NO_KEY, l_tileset_no),
			l_key_pos.first, l_key_pos.second, l_level.is_key_hidden());
		if (l_level.is_key_in_block()) {
			draw_tile(p_rnd, m_gfx.get_meta_tile(c::MD_BYTE_NO_BLOCK_BROWN,
				l_tileset_no), l_key_pos.first, l_key_pos.second, true);
		}

	}

	// draw items
	const auto& l_items = l_level.get_items();
	for (const auto& item : l_items) {
		byte l_no = item.get_item_no();
		auto l_pos = item.get_position();
		draw_tile(p_rnd, m_gfx.get_item_tile(item.get_item_no(), l_tileset_no),
			l_pos.first, l_pos.second,
			skc::Level::is_item_hidden(item.get_element_no()));
		if (skc::Level::is_item_in_block(item.get_element_no()))
			draw_tile(p_rnd, m_gfx.get_meta_tile(c::MD_BYTE_NO_BLOCK_BROWN,
				l_tileset_no), l_pos.first, l_pos.second, true);
	}

	// draw enemies
	const auto& l_enemies = l_level.get_enemies();
	for (const auto& enemy : l_enemies) {
		byte l_no = enemy.get_element_no();
		auto l_pos = enemy.get_position();
		draw_tile(p_rnd, m_gfx.get_enemy_tile(enemy.get_element_no(), l_tileset_no),
			l_pos.first, l_pos.second);
	}

	if (is_selected_index_valid()) {
		std::pair<int, int> l_pos{ 0,0 };
		if (m_selected_type == c::ELM_TYPE_ITEM)
			l_pos = l_level.get_items().at(get_selected_index()).get_position();
		else if (m_selected_type == c::ELM_TYPE_ENEMY)
			l_pos = l_level.get_enemies().at(get_selected_index()).get_position();

		klib::gfx::draw_rect(p_rnd,
			l_pos.first * c::TILE_GFX_SIZE, l_pos.second * c::TILE_GFX_SIZE,
			c::TILE_GFX_SIZE, c::TILE_GFX_SIZE, SDL_Color{ 255, 255, 0 }, 2);
	}

	SDL_SetRenderTarget(p_rnd, nullptr);
}

int skc::SKC_Main_window::get_tile_w(int p_screen_h) const {
	return std::max<int>(1, p_screen_h / c::LEVEL_H);
}

void skc::SKC_Main_window::draw_ui(const SKC_Config& p_config) {
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	this->draw_ui_level_window(p_config);
	this->draw_ui_item_window(p_config);
	this->draw_ui_enemy_window(p_config);

	ImGui::Render();
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
}

void skc::SKC_Main_window::draw_ui_item_window(const SKC_Config& p_config) {
	ImGui::Begin("Items");

	draw_tile_picker(p_config, c::ELM_TYPE_ITEM);

	ImGui::End();
}

void skc::SKC_Main_window::draw_ui_enemy_window(const SKC_Config& p_config) {
	ImGui::Begin("Enemies");

	draw_tile_picker(p_config, c::ELM_TYPE_ENEMY);

	ImGui::End();
}

void skc::SKC_Main_window::draw_ui_level_window(const SKC_Config& p_config) {
	std::string l_level_str{ "Level " + std::to_string(m_current_level + 1) + "###lvl" };
	ImGui::Begin(l_level_str.c_str());

	if (ImGui::Button("Save xml")) {
		for (std::size_t i{ 0 }; i < m_levels.size(); ++i)
			save_level_xml(m_levels.at(i), "./xml", "level-" + std::to_string(i + 1) + ".xml");
	}

	ImGui::Separator();

	draw_tile_picker(p_config, c::ELM_TYPE_METADATA);

	ImGui::End();
}

void skc::SKC_Main_window::draw_tile_picker(const SKC_Config& p_config, std::size_t p_element_types) {
	const auto& l_tile_picker = p_config.get_tile_picker(p_element_types);

	std::size_t l_tileset_no{ p_config.get_level_tileset(m_current_level) };

	for (const auto& kv : l_tile_picker) {
		ImGui::Text(kv.first.c_str());
		for (byte n : kv.second) {
			bool l_is_selected{ m_selected_type == p_element_types && m_selected == n };
			bool l_is_constellation = (p_element_types == c::ELM_TYPE_METADATA &&
				skc::Level::is_item_constellation(n));

			if (!l_is_constellation || is_valid_constellation(n)) {
				if (l_is_selected)
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 1.0f, 1.0f, 0.0f, 1.0f });
				ImGui::PushID(n);

				if (ImGui::ImageButton(m_gfx.get_tile(p_element_types, n, l_tileset_no),
					{ l_is_constellation ? 1.5f * c::TILE_GFX_SIZE : c::TILE_GFX_SIZE,
					c::TILE_GFX_SIZE })) {
					m_selected = n;
					m_selected_type = p_element_types;
				}

				if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
					ImGui::SetTooltip(p_config.get_description(p_element_types, n).c_str());

				ImGui::PopID();
				if (l_is_selected)
					ImGui::PopStyleColor();

				ImGui::SameLine();
			}
		}
		ImGui::NewLine();
	}

}

void skc::SKC_Main_window::draw(SDL_Renderer* p_rnd, const SKC_Config& p_config, int p_w, int p_h) {
	this->generate_texture(p_rnd, p_config);

	int l_tile_w{ get_tile_w(p_h) };
	int l_screen_w = c::LEVEL_W * l_tile_w;
	int l_screen_h = c::LEVEL_H * l_tile_w;

	SDL_SetRenderDrawColor(p_rnd, 126, 126, 255, 0);
	SDL_RenderClear(p_rnd);

	klib::gfx::blit_full_spec(p_rnd, m_texture,
		0, 0, l_screen_w, l_screen_h,
		0, 0, c::LEVEL_W * c::TILE_GFX_SIZE, c::LEVEL_H * c::TILE_GFX_SIZE);

	this->draw_ui(p_config);
}

void skc::SKC_Main_window::save_nes_file(const std::string& p_file_path, const SKC_Config& p_config) const {
	std::vector<byte> l_output{ p_config.get_rom_data() };
	std::vector<std::size_t> l_item_offsets, l_enemy_offsets;
	std::size_t l_item_offset{ 0 }, lenemy_offset{ 0 };

	std::vector<byte> l_item_data, l_enemy_data, l_block_data;
	for (std::size_t i{ 0 }; i < m_levels.size(); ++i) {
		auto l_data = m_levels[i].get_item_bytes();
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
			l_index < static_cast<int>(l_level.get_enemies().size()));
}
