#include "SKC_Main_window.h"
#include "./../common/klib/klib_gfx.h"
#include "./../common/klib/klib_file.h"
#include "./../common/klib/klib_util.h"
#include "./../common/klib/IPS_Patch.h"
#include "./../skc_util/Xml_helper.h"
#include "./../skc_util/Imgui_helper.h"
#include "./../skc_constants/Constants_level.h"
#include "./../common/imgui/imgui.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"
#include <algorithm>
#include <map>
#include <utility>
#include <vector>

void skc::SKC_Main_window::draw_ui_selected_tile_window(const SKC_Config& p_config) {
	ImGui::Begin("Selected Element");
	int l_index{ get_selected_index() };

	if (is_selected_index_valid()) {
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

			auto l_pos{ l_items.at(l_index).get_position() };
			int l_x{ l_pos.first };
			int l_y{ l_pos.second };

			if (ImGui::SliderInt("x-pos", &l_x, 0, c::LEVEL_W - 1)) {
				l_level.set_item_position(l_index, std::make_pair(l_x, l_y));
			}
			if (ImGui::SliderInt("y-pos", &l_y, 0, c::LEVEL_H - 1)) {
				l_level.set_item_position(l_index, std::make_pair(l_x, l_y));
			}

		}
		else if (m_selected_type == c::ELM_TYPE_METADATA)
			draw_ui_selected_metadata(p_config);
	}
	else {
		ImGui::Text("No element selected");
	}
	ImGui::End();
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

	auto l_cpos{ get_metadata_tile_position(l_index) };
	auto l_pos_x = skc::imgui::slider("x-pos", l_cpos.first,
		0, c::LEVEL_W - 1);
	auto l_pos_y = skc::imgui::slider("y-pos", l_cpos.second,
		0, c::LEVEL_H - 1);

	if (l_pos_x)
		set_metadata_tile_position(l_index, std::make_pair(l_pos_x.value(), l_cpos.second), p_config);
	else if (l_pos_y)
		set_metadata_tile_position(l_index, std::make_pair(l_cpos.first, l_pos_y.value()), p_config);

	if (l_index == c::MD_BYTE_NO_SPAWN01)
		draw_ui_selected_mirror(0, p_config);
	else if (l_index == c::MD_BYTE_NO_SPAWN02)
		draw_ui_selected_mirror(1, p_config);
}
