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

	imgui::draw_enemy_set_interface(p_sets, p_config, p_gfx, p_available_monster_no, p_editor, m_current_set_index, m_current_monster_index);

	ImGui::End();
}

