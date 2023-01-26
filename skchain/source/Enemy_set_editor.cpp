#include "Enemy_set_editor.h"
#include "./skc_util/Imgui_helper.h"
#include "./skc_constants/Constants_application.h"
#include "./skc_constants/Constants_level.h"

#include <algorithm>

void skc::Enemy_set_editor::draw_ui(std::vector<std::vector<byte>>& p_sets,
	const SKC_Config& p_config,
	const SKC_Gfx& p_gfx,
	byte p_available_monster_no,
	const Enemy_editor& p_editor) {

	imgui::window("Mirror Enemy Sets", c::WIN_ENEMYSET_X, c::WIN_ENEMYSET_Y,
		c::WIN_ENEMYSET_W, c::WIN_ENEMYSET_H);

	auto l_set_no{ imgui::slider<std::size_t>("Enemy Set", m_current_set_index + 1, 1, p_sets.size()) };
	if (l_set_no)
		m_current_set_index = l_set_no.value() - 1;

	auto& l_set{ p_sets[m_current_set_index] };

	for (std::size_t i{ 0 }; i < l_set.size(); ++i) {
		std::string l_id{ "mse" + std::to_string(i) };
		byte l_enemy_no{ l_set[i] };
		bool l_is_selected{ m_current_monster_index == i };

		if (l_is_selected)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 1.0f, 1.0f, 0.0f, 1.0f });
		ImGui::PushID(l_id.c_str());

		if (ImGui::ImageButton(p_gfx.get_enemy_tile(l_enemy_no, 0), { c::TILE_GFX_SIZE, c::TILE_GFX_SIZE }))
			m_current_monster_index = i;

		imgui::tooltip_text(p_config.get_description(c::ELM_TYPE_ENEMY, l_enemy_no));

		ImGui::PopID();
		if (l_is_selected)
			ImGui::PopStyleColor();

		ImGui::SameLine();
	}

	ImGui::NewLine();

	ImGui::Separator();

	if (m_current_monster_index < l_set.size()) {
		byte l_enemy_no{ l_set[m_current_monster_index] };

		ImGui::Separator();

		std::string l_enemy_description{ "Selected enemy: " + p_config.get_description(c::ELM_TYPE_ENEMY, l_enemy_no) };
		ImGui::Text(l_enemy_description.c_str());

		ImGui::Separator();

		auto l_new_enemy_no{ imgui::slider_enemy_properties(l_enemy_no, p_editor) };
		if (l_new_enemy_no)
			l_set[m_current_monster_index] = l_new_enemy_no.value();

		ImGui::Separator();

		if (imgui::button("Move Left") && m_current_monster_index > 0) {
			std::swap(l_set[m_current_monster_index - 1], l_set[m_current_monster_index]);
			--m_current_monster_index;
		}
		ImGui::SameLine();
		if (imgui::button("Move Right") && m_current_monster_index < l_set.size() - 1) {
			std::swap(l_set[m_current_monster_index + 1], l_set[m_current_monster_index]);
			++m_current_monster_index;
		}
		ImGui::SameLine();
		if (imgui::button("Delete") && l_set.size() > 0) {
			l_set.erase(begin(l_set) + m_current_monster_index);
			if (m_current_monster_index > 0)
				--m_current_monster_index;
		}
	}
	else
		ImGui::Text("No enemy selected");

	if (p_available_monster_no == 0)
		ImGui::Text("Select new enemies from the enemy element picker");
	else if (imgui::button("Add from Element Picker", c::COLOR_STYLE_NORMAL,
		p_config.get_description(c::ELM_TYPE_ENEMY, p_available_monster_no))) {
		l_set.push_back(p_available_monster_no);
	}

	ImGui::End();
}
