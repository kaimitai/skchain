#include "SKC_Main_window.h"
#include "./../common/klib/klib_gfx.h"
#include "./../common/klib/klib_file.h"
#include "./../common/klib/klib_util.h"
#include "./../common/klib/IPS_Patch.h"
#include "./../skc_util/Rom_expander.h"
#include "./../skc_util/Xml_helper.h"
#include "./../skc_util/Imgui_helper.h"
#include "./../skc_constants/Constants_level.h"
#include "./../skc_constants/Constants_application.h"
#include "./../skc_constants/Constants_color.h"
#include "./../common/imgui/imgui.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"
#include <algorithm>
#include <map>
#include <utility>
#include <vector>

void skc::SKC_Main_window::draw_ui(SKC_Config& p_config,
	const klib::User_input& p_input) {
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	this->draw_ui_main_window(p_config, p_input);
	this->draw_ui_tile_picker_window(p_config);
	this->draw_ui_selected_tile_window(p_config);
	if (m_schedule_win_index)
		draw_ui_metadata_drop_schedules();
	if (m_enemy_sets_win_index)
		draw_ui_metadata_enemy_sets(p_config);

	this->draw_ui_level_board(p_config, p_input);

	ImGui::Render();
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
}

void skc::SKC_Main_window::draw_ui_level_board(SKC_Config& p_config, const klib::User_input& p_input) {
	auto& l_level{ get_level() };
	std::string l_level_str{ "Level " + std::to_string(m_current_level + 1) + "###lvl" };

	//ImGui::Begin(l_level_str.c_str(), nullptr, ImGuiWindowFlags_NoScrollWithMouse);
	imgui::window(l_level_str, c::WIN_BOARD_X, c::WIN_BOARD_Y, c::WIN_BOARD_W, c::WIN_BOARD_H);
	bool l_win_focused{ ImGui::IsWindowFocused() };

	ImVec2 l_wmin = ImGui::GetWindowContentRegionMin();
	ImVec2 l_wmax = ImGui::GetWindowContentRegionMax();

	constexpr float l_ow{ static_cast<float>(c::LEVEL_W * c::TILE_GFX_SIZE) };
	constexpr float l_oh{ static_cast<float>(c::LEVEL_H * c::TILE_GFX_SIZE) };
	float l_iw = (l_wmax.x - l_wmin.x);
	float l_ih = (l_wmax.y - l_wmin.y);

	float l_scale_w = l_iw / l_ow;
	float l_scale_h = l_ih / l_oh;
	float scale = std::min(l_scale_w, l_scale_h);

	bool l_ctrl{ p_input.is_ctrl_pressed() };
	bool l_shift{ p_input.is_shift_pressed() };
	bool l_lclick{ p_input.mouse_held(true) };
	bool l_rclick{ p_input.mouse_held(false) || p_input.is_pressed(SDL_SCANCODE_1) ||
	p_input.is_pressed(SDL_SCANCODE_2) || p_input.is_pressed(SDL_SCANCODE_3) };
	bool l_click{ l_lclick || l_rclick };
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Image(m_texture, { l_ow * scale, l_oh * scale });

	if (l_click && ImGui::IsItemHovered()) {
		int lx = static_cast<int>(ImGui::GetItemRectMin().x);
		int ly = static_cast<int>(ImGui::GetItemRectMin().y);
		int lw = static_cast<int>(ImGui::GetItemRectMax().x - ImGui::GetItemRectMin().x);
		int lh = static_cast<int>(ImGui::GetItemRectMax().y - ImGui::GetItemRectMin().y);

		int tx = c::LEVEL_W * (static_cast<int>(io.MousePos.x) - lx) / lw;
		int ty = c::LEVEL_H * (static_cast<int>(io.MousePos.y) - ly) / lh;
		auto l_tpos{ std::make_pair(tx,ty) };

		bool l_valid_tile{ tx >= 0 && tx < c::LEVEL_W&&
			ty >= 0 && ty < c::LEVEL_H };

		if (l_rclick && l_valid_tile)
			right_click(l_tpos, p_config, p_input);
		else if (l_lclick && l_valid_tile && l_win_focused) {
			if (l_shift)
				shift_click(l_tpos, p_config);
			else
				left_click(l_tpos, p_config);
		}
	}

	ImGui::Separator();

	ImGui::PushAllowKeyboardFocus(false);
	auto l_tileset{ imgui::slider<int>("Tileset", l_level.get_tileset_no(), 0, 2) };
	if (l_tileset)
		l_level.set_tileset_no(l_tileset.value());
	auto l_time_decrease{ imgui::slider<int>("Time Decrease Rate", l_level.get_time_decrease_rate(), 0, 15) };
	if (l_time_decrease)
		l_level.set_time_decrease_rate(l_time_decrease.value());
	auto l_spawn_life{ imgui::slider<int>("Demonhead/Saramandor Lifetime", l_level.get_spawn_enemy_lifetime(), 0, 255) };
	if (l_spawn_life)
		l_level.set_spawn_enemy_lifetime(l_spawn_life.value());

	ImGui::Separator();

	if (imgui::button("Clear Level Data", 2, "Clears the level. Hold Ctrl to use.") && l_ctrl) {
		get_level() = skc::Level();
		get_level().set_right_wall();
		reset_selections(m_current_level);
	}

	if (l_level.has_constellation()) {
		ImGui::SameLine();
		if (imgui::button("Remove constellation"))
			l_level.delete_constellation();
	}

	ImGui::Separator();

	static std::vector<std::string> ls_toggles{
	c::TXT_GRID, c::TXT_META, c::TXT_ITEMS, c::TXT_ENEMIES
	};

	ImGui::Text("Draw");
	for (std::size_t i{ 0 }; i < ls_toggles.size(); ++i) {
		auto l_toggled{ imgui::checkbox(ls_toggles[i], m_render_toggles[i]) };
		if (l_toggled)
			m_render_toggles[i] = !m_render_toggles[i];
		ImGui::SameLine();
	}

	ImGui::PopAllowKeyboardFocus();

	ImGui::End();
}

void skc::SKC_Main_window::draw_ui_main_window(SKC_Config& p_config, const klib::User_input& p_input) {
	bool l_ctrl = ImGui::GetIO().KeyCtrl;

	imgui::window("Main", c::WIN_MAIN_X, c::WIN_MAIN_Y, c::WIN_MAIN_W, c::WIN_MAIN_H);

	if (ImGui::Button("Save xml"))
		save_xml_files(p_config);
	ImGui::SameLine();
	if (ImGui::Button("Save IPS"))
		save_ips_file(p_config);
	ImGui::SameLine();
	if (ImGui::Button("Save NES"))
		save_nes_file(p_config, p_input.is_shift_pressed());

	if (p_config.get_region_code() == c::REGION_US &&
		!is_rom_expanded()) {
		ImGui::SameLine();
		if (imgui::button("Expand ROM", 1, "Change ROM mapper. Holdt Ctrl to use") && l_ctrl)
			expand_rom_data(p_config);
	}

	if (imgui::button("Load xml", c::COLOR_STYLE_NORMAL, "Hold ctrl to use this button")
		&& l_ctrl)
		load_xml_files(p_config);

	ImGui::Separator();
	ImGui::Text("Metadata Editors");
	if (imgui::button("Drop Schedules", m_schedule_win_index ? c::COLOR_STYLE_ORANGE : c::COLOR_STYLE_NORMAL)) {
		if (m_schedule_win_index)
			m_schedule_win_index = std::nullopt;
		else
			m_schedule_win_index = 0;
	}
	ImGui::SameLine();
	if (imgui::button("Enemy Sets", m_enemy_sets_win_index ? c::COLOR_STYLE_ORANGE : c::COLOR_STYLE_NORMAL)) {
		if (m_enemy_sets_win_index)
			m_enemy_sets_win_index = std::nullopt;
		else
			m_enemy_sets_win_index = std::make_pair(0, 0);
	}

	ImGui::Separator();

	auto l_lvl_no = imgui::slider<std::size_t>("Level " + std::to_string(m_current_level + 1) +
		"/" + std::to_string(m_levels.size()) + "###lvl", m_current_level + 1,
		1, m_levels.size());
	if (l_lvl_no)
		m_current_level = l_lvl_no.value() - 1;

	ImGui::Separator();

	static std::vector<ImVec4> ls_color_codes{
		c::COLI_WHITE,
		c::COLI_YELLOW,
		c::COLI_GREEN,
		c::COLI_RED
	};

	const auto& lr_msgs{ p_config.get_messages() };
	for (const auto& l_msg : lr_msgs) {
		ImGui::PushStyleColor(ImGuiCol_Text, ls_color_codes[l_msg.second]);
		ImGui::Text(l_msg.first.c_str());
		ImGui::PopStyleColor();
	}

	ImGui::End();
}

void skc::SKC_Main_window::draw_ui_tile_picker_window(SKC_Config& p_config) {
	imgui::window("Element Picker", c::WIN_EP_X, c::WIN_EP_Y, c::WIN_EP_W, c::WIN_EP_H);

	std::size_t l_tmp_selected_type = m_selected_type;

	if (imgui::button(c::TXT_META, l_tmp_selected_type == c::ELM_TYPE_METADATA ? c::COLOR_STYLE_ORANGE : c::COLOR_STYLE_NORMAL))
		m_selected_type = c::ELM_TYPE_METADATA;
	ImGui::SameLine();
	if (imgui::button(c::TXT_ITEMS, l_tmp_selected_type == c::ELM_TYPE_ITEM ? c::COLOR_STYLE_ORANGE : c::COLOR_STYLE_NORMAL))
		m_selected_type = c::ELM_TYPE_ITEM;
	ImGui::SameLine();
	if (imgui::button(c::TXT_ENEMIES, l_tmp_selected_type == c::ELM_TYPE_ENEMY ? c::COLOR_STYLE_ORANGE : c::COLOR_STYLE_NORMAL))
		m_selected_type = c::ELM_TYPE_ENEMY;

	ImGui::Separator();

	draw_tile_picker(p_config, m_selected_type);

	ImGui::End();
}

void skc::SKC_Main_window::draw_ui_selected_mirror(std::size_t p_mirror_no, const SKC_Config& p_config) {
	auto& l_level{ get_level() };
	auto l_tileset = l_level.get_tileset_no();

	ImGui::Separator();
	ImGui::Text(c::TXT_PROPERTIES);

	byte l_spawn_index = l_level.get_spawn_schedule(p_mirror_no);
	byte l_spawn_nmi_index = l_level.get_spawn_enemies(p_mirror_no);

	auto l_schedule_no = skc::imgui::slider<int>("Schedule",
		l_spawn_index + 1, 1, static_cast<int>(m_drop_schedules.size()));
	if (l_schedule_no)
		l_level.set_spawn_schedule(p_mirror_no, l_schedule_no.value() - 1);
	auto l_nmi_set_no = skc::imgui::slider<int>("Enemy Set",
		l_spawn_nmi_index + 1, 1, static_cast<int>(m_drop_enemies.size()));
	if (l_nmi_set_no)
		l_level.set_spawn_enemies(p_mirror_no, l_nmi_set_no.value() - 1);

	ImGui::Separator();

	imgui::draw_drop_schedule_interface("ds", m_drop_schedules, l_spawn_index);

	ImGui::Separator();
	ImGui::Text("Enemy Set");

	imgui::draw_enemy_set_interface(m_drop_enemies, p_config, m_gfx,
		m_selected_picker_tile[c::ELM_TYPE_ENEMY],
		p_config.get_enemy_editor(), l_spawn_nmi_index, m_sel_es_index, l_tileset);
}

void skc::SKC_Main_window::draw_tile_picker(const SKC_Config& p_config, std::size_t p_element_types) {
	const auto& l_tile_picker = p_config.get_tile_picker(p_element_types);

	std::size_t l_tileset_no{ p_config.get_level_tileset(m_current_level, get_level().get_tileset_no()) };

	for (const auto& kv : l_tile_picker) {
		ImGui::Text(kv.first.c_str());
		for (byte n : kv.second) {
			bool l_is_selected{ m_selected_type == p_element_types && get_tile_selection() == n };
			bool l_is_constellation = (p_element_types == c::ELM_TYPE_METADATA &&
				skc::Level::is_item_constellation(n));

			if (!l_is_constellation || is_valid_constellation(n)) {
				if (l_is_selected)
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 1.0f, 1.0f, 0.0f, 1.0f });
				ImGui::PushID(n);

				if (ImGui::ImageButton(m_gfx.get_tile(p_element_types, n, l_tileset_no),
					{ l_is_constellation ? 1.5f * c::TILE_GFX_SIZE : c::TILE_GFX_SIZE,
					c::TILE_GFX_SIZE })) {
					set_tile_selection(n);
				}

				imgui::tooltip_text(p_config.get_description(p_element_types, n));

				ImGui::PopID();
				if (l_is_selected)
					ImGui::PopStyleColor();

				ImGui::SameLine();
			}
		}
		ImGui::NewLine();
	}

	if (p_element_types == c::ELM_TYPE_METADATA) {
		auto iter = m_meta_tiles.find(m_current_level);
		if (iter != end(m_meta_tiles)) {
			ImGui::Text("Meta Tiles");
			for (std::size_t i{ 0 }; i < iter->second.size(); ++i) {
				std::size_t l_meta_elm_no{ iter->second[i].first };
				std::size_t l_tile_gfx_no{ p_config.get_meta_tile_tile_no(l_meta_elm_no) };
				byte l_id{ static_cast<byte>(c::MD_BYTE_NO_META_TILE_MIN + static_cast<byte>(i)) };
				bool l_is_selected{ m_selected_type == p_element_types &&
					get_tile_selection() == l_id };

				if (l_is_selected)
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 1.0f, 1.0f, 0.0f, 1.0f });
				ImGui::PushID(l_id);

				if (ImGui::ImageButton(m_gfx.get_absolute_tile(l_tile_gfx_no, l_tileset_no),
					{ c::TILE_GFX_SIZE, c::TILE_GFX_SIZE })) {
					set_tile_selection(l_id);
				}

				imgui::tooltip_text(p_config.get_meta_tile_description(l_meta_elm_no));

				ImGui::PopID();
				if (l_is_selected)
					ImGui::PopStyleColor();

				ImGui::SameLine();
			}
		}
	}

}

void skc::SKC_Main_window::draw_ui_selected_tile_window(const SKC_Config& p_config) {
	int l_index{ get_selected_index() };
	bool l_index_valid{ is_selected_index_valid() };

	std::string l_stw_desc{ " Selected Element" };
	if (l_index_valid) {
		if (m_selected_type == c::ELM_TYPE_ITEM)
			l_stw_desc += " (Item #" + std::to_string(l_index + 1) + "/" + std::to_string(get_selected_index_count()) + ")";
		else if (m_selected_type == c::ELM_TYPE_ENEMY)
			l_stw_desc += " (Enemy #" + std::to_string(l_index + 1) + "/" + std::to_string(get_selected_index_count()) + ")";
		else
			l_stw_desc += " (Metadata)";
	}

	l_stw_desc += "###stw";

	imgui::window(l_stw_desc, c::WIN_STW_X, c::WIN_STW_Y, c::WIN_STW_W, c::WIN_STW_H);

	if (l_index_valid) {
		auto& l_level{ get_level() };
		byte l_tileset{ l_level.get_tileset_no() };

		if (m_selected_type == c::ELM_TYPE_ITEM) {
			const auto& l_items{ get_level().get_items() };
			auto l_elm_no{ l_items.at(get_selected_index()).get_element_no() };
			auto l_item_no{ l_items.at(get_selected_index()).get_item_no() };

			ImGui::Image(m_gfx.get_tile(c::ELM_TYPE_ITEM, l_items.at(l_index).get_item_no(),
				p_config.get_level_tileset(m_current_level, l_tileset)),
				{ 2 * c::TILE_GFX_SIZE, 2 * c::TILE_GFX_SIZE });

			std::string l_desc{ "Item #" + std::to_string(l_item_no) + ": "
			+ p_config.get_description(c::ELM_TYPE_ITEM, l_item_no) };

			ImGui::Text(l_desc.c_str());

			auto l_elm_slider_no{ imgui::slider<byte>("Code", l_elm_no, 1, c::ITEM_COPY_INDICATOR_MIN - 1) };
			if (l_elm_slider_no)
				l_level.set_item_element_no(l_index, l_elm_slider_no.value());

			auto l_newpos{ imgui::position_sliders(l_items.at(l_index).get_position()) };
			if (l_newpos)
				l_level.set_item_position(l_index, l_newpos.value());

			ImGui::Separator();
			ImGui::Text(c::TXT_PROPERTIES);

			bool l_hidden{ skc::Level::is_item_hidden(l_elm_no) };
			bool l_in_block{ skc::Level::is_item_in_block(l_elm_no) };

			if (ImGui::Checkbox("Hidden", &l_hidden)) {
				l_level.set_item_hidden(l_index, l_hidden);
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("In Block", &l_in_block)) {
				l_level.set_item_in_block(l_index, l_in_block);
			}

		}
		else if (m_selected_type == c::ELM_TYPE_ENEMY)
			draw_ui_selected_enemy(p_config);
		else if (m_selected_type == c::ELM_TYPE_METADATA)
			draw_ui_selected_metadata(p_config);

		if (m_selected_type != c::ELM_TYPE_METADATA) {
			ImGui::Separator();
			if (imgui::button("Delete", c::COLOR_STYLE_RED))
				delete_selected_index();
		}
	}
	else {
		ImGui::Text("No element selected");
	}
	ImGui::End();
}

void skc::SKC_Main_window::draw_ui_selected_enemy(const SKC_Config& p_config) {
	int l_index{ get_selected_index() };
	auto& l_level{ get_level() };
	const auto& l_enemies{ l_level.get_enemies() };
	byte l_enemy_no{ l_enemies.at(l_index).get_element_no() };
	std::size_t l_tileset{ p_config.get_level_tileset(m_current_level, l_level.get_tileset_no()) };

	ImGui::Image(m_gfx.get_tile(c::ELM_TYPE_ENEMY, l_enemy_no,
		l_tileset),
		{ 2 * c::TILE_GFX_SIZE, 2 * c::TILE_GFX_SIZE });

	std::string l_desc{ "Enemy #" + std::to_string(l_enemy_no) + ": "
			+ p_config.get_description(c::ELM_TYPE_ENEMY, l_enemy_no) };
	ImGui::Text(l_desc.c_str());

	auto l_elm_slider_no{ imgui::slider<byte>("Code", l_enemy_no, 1, 0x83) };
	if (l_elm_slider_no)
		l_level.set_enemy_no(l_index, l_elm_slider_no.value());

	auto l_newpos{ imgui::position_sliders(l_enemies.at(l_index).get_position()) };
	if (l_newpos)
		l_level.set_enemy_position(l_index, l_newpos.value());

	ImGui::Separator();
	ImGui::Text(c::TXT_PROPERTIES);

	auto l_new_enemy_no{ imgui::slider_enemy_properties(l_enemy_no, p_config.get_enemy_editor()) };
	if (l_new_enemy_no)
		l_level.set_enemy_no(l_index, l_new_enemy_no.value());
}

void skc::SKC_Main_window::draw_ui_selected_metadata(const SKC_Config& p_config) {
	int l_index{ get_selected_index() };
	bool l_is_meta_tile{ l_index >= c::MD_BYTE_NO_META_TILE_MIN };
	std::size_t l_meta_tile_no{ l_is_meta_tile ?
	m_meta_tiles.at(m_current_level).at(static_cast<std::size_t>(l_index - c::MD_BYTE_NO_META_TILE_MIN)).first :
	0 };
	bool l_meta_movable{ !l_is_meta_tile || p_config.get_meta_tile_movable(l_meta_tile_no) };

	auto& l_level{ get_level() };
	byte l_tileset{ l_level.get_tileset_no() };
	std::string l_desc{ l_is_meta_tile ?
		p_config.get_meta_tile_description(l_meta_tile_no) :
		p_config.get_description(c::ELM_TYPE_METADATA, l_index) };

	ImGui::Image(l_is_meta_tile ?
		m_gfx.get_absolute_tile(p_config.get_meta_tile_tile_no(l_meta_tile_no), l_tileset) :
		m_gfx.get_tile(c::ELM_TYPE_METADATA, l_index, l_tileset),
		{ 2 * c::TILE_GFX_SIZE, 2 * c::TILE_GFX_SIZE });

	ImGui::Text(l_desc.c_str());
	auto l_newpos{ imgui::position_sliders(get_metadata_tile_position(l_index), !l_meta_movable) };
	if (l_newpos)
		set_metadata_tile_position(l_index, l_newpos.value(), p_config);

	if (l_index == c::MD_BYTE_NO_KEY) {
		ImGui::Separator();
		ImGui::Text(c::TXT_PROPERTIES);

		auto l_khidden{ imgui::checkbox("Hidden", l_level.is_key_hidden()) };
		ImGui::SameLine();
		auto l_kcovered{ imgui::checkbox("In Block", l_level.is_key_in_block()) };
		if (l_khidden)
			l_level.set_key_hidden(l_khidden.value());
		if (l_kcovered)
			l_level.set_key_in_block(l_kcovered.value());
	}

	if (l_index == c::MD_BYTE_NO_SPAWN01)
		draw_ui_selected_mirror(0, p_config);
	else if (l_index == c::MD_BYTE_NO_SPAWN02)
		draw_ui_selected_mirror(1, p_config);

	ImGui::Separator();
	if (imgui::button("Hide", 2)) {
		set_metadata_tile_position(l_index, std::make_pair(0, -1), p_config);
	}
}

void skc::SKC_Main_window::draw_ui_metadata_drop_schedules(void) {
	imgui::window("Mirror Drop Schedules", c::WIN_DS_X, c::WIN_DS_Y, c::WIN_DS_W, c::WIN_DS_H);

	auto l_nv{ imgui::slider("Schedule###mdds",
		static_cast<int>(m_schedule_win_index.value() + 1),
		1, static_cast<int>(m_drop_schedules.size())) };

	if (l_nv)
		m_schedule_win_index = static_cast<std::size_t>(l_nv.value() - 1);

	imgui::draw_drop_schedule_interface("md", m_drop_schedules, m_schedule_win_index.value());

	ImGui::End();
}

void skc::SKC_Main_window::draw_ui_metadata_enemy_sets(const SKC_Config& p_config) {
	imgui::window("Mirror Enemy Sets", c::WIN_ENEMYSET_X, c::WIN_ENEMYSET_Y,
		c::WIN_ENEMYSET_W, c::WIN_ENEMYSET_H);

	auto& l_params{ m_enemy_sets_win_index.value() };

	auto l_set_no{ imgui::slider<std::size_t>("Enemy Set", l_params.first + 1, 1, m_drop_enemies.size()) };
	if (l_set_no)
		l_params.first = l_set_no.value() - 1;

	imgui::draw_enemy_set_interface(m_drop_enemies, p_config, m_gfx,
		m_selected_picker_tile[c::ELM_TYPE_ENEMY], p_config.get_enemy_editor(),
		l_params.first, l_params.second, get_level().get_tileset_no());

	ImGui::End();
}
