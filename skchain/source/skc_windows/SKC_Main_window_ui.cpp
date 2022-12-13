#include "SKC_Main_window.h"
#include "./../common/klib/klib_gfx.h"
#include "./../common/klib/klib_file.h"
#include "./../common/klib/klib_util.h"
#include "./../common/klib/IPS_Patch.h"
#include "./../skc_util/Xml_helper.h"
#include "./../skc_util/Imgui_helper.h"
#include "./../skc_constants/Constants_level.h"
#include "./../skc_constants/Constants_application.h"
#include "./../common/imgui/imgui.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"
#include <algorithm>
#include <map>
#include <utility>
#include <vector>

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

	ImGui::Begin(l_stw_desc.c_str());

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
			ImGui::Separator();

			bool l_hidden{ skc::Level::is_item_hidden(l_elm_no) };
			bool l_in_block{ skc::Level::is_item_in_block(l_elm_no) };

			if (ImGui::Checkbox("Hidden", &l_hidden)) {
				l_level.set_item_hidden(l_index, l_hidden);
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("In Block", &l_in_block)) {
				l_level.set_item_in_block(l_index, l_in_block);
			}

			ImGui::Separator();

			auto l_newpos{ imgui::position_sliders(l_items.at(l_index).get_position()) };
			if (l_newpos)
				l_level.set_item_position(l_index, l_newpos.value());
		}
		else if (m_selected_type == c::ELM_TYPE_ENEMY)
			draw_ui_selected_enemy(p_config);
		else if (m_selected_type == c::ELM_TYPE_METADATA)
			draw_ui_selected_metadata(p_config);
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

	ImGui::Separator();
	auto l_newpos{ imgui::position_sliders(l_enemies.at(l_index).get_position()) };
	if (l_newpos)
		l_level.set_enemy_position(l_index, l_newpos.value());
}

void skc::SKC_Main_window::draw_ui_selected_metadata(const SKC_Config& p_config) {
	int l_index{ get_selected_index() };
	bool l_is_meta_tile{ l_index >= c::MD_BYTE_NO_META_TILE_MIN };
	std::size_t l_meta_tile_no{ l_is_meta_tile ?
	m_meta_tiles.at(m_current_level).at(l_index - c::MD_BYTE_NO_META_TILE_MIN).first :
	0 };

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
	ImGui::Separator();

	if (l_index == c::MD_BYTE_NO_KEY) {
		auto l_khidden{ imgui::checkbox("Hidden", l_level.is_key_hidden()) };
		ImGui::SameLine();
		auto l_kcovered{ imgui::checkbox("In Block", l_level.is_key_in_block()) };
		if (l_khidden)
			l_level.set_key_hidden(l_khidden.value());
		if (l_kcovered)
			l_level.set_key_in_block(l_kcovered.value());
		ImGui::Separator();
	}

	auto l_newpos{ imgui::position_sliders(get_metadata_tile_position(l_index)) };
	if (l_newpos)
		set_metadata_tile_position(l_index, l_newpos.value(), p_config);

	if (l_index == c::MD_BYTE_NO_SPAWN01)
		draw_ui_selected_mirror(0, p_config);
	else if (l_index == c::MD_BYTE_NO_SPAWN02)
		draw_ui_selected_mirror(1, p_config);
}
