#include "Imgui_helper.h"
#include "./../common/imgui/imgui.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"
#include "./../skc_constants/Constants_application.h"
#include "./../skc_constants/Constants_level.h"

std::optional<int> skc::imgui::slider(const std::string& p_label,
	int p_value, int p_min, int p_max) {
	int l_value{ p_value };
	if (ImGui::SliderInt(p_label.c_str(), &l_value, p_min, p_max))
		return l_value;
	else
		return std::nullopt;
}

std::optional<bool> skc::imgui::checkbox(const std::string& p_label, bool p_value) {
	bool l_value{ p_value };
	if (ImGui::Checkbox(p_label.c_str(), &l_value))
		return l_value;
	else
		return std::nullopt;
}

std::optional<position> skc::imgui::position_sliders(const position& p_value) {
	auto l_x{ slider(c::TXT_X_POS, p_value.first, 0, c::LEVEL_W - 1) };
	auto l_y{ slider(c::TXT_Y_POS, p_value.second, 0, c::LEVEL_H - 1) };

	if (l_x || l_y) {
		int r_x{ l_x ? l_x.value() : p_value.first };
		int r_y{ l_y ? l_y.value() : p_value.second };
		return std::make_pair(r_x, r_y);
	}
	else
		return std::nullopt;
}

bool skc::imgui::button(const std::string& p_label, const std::string& p_tooltip_text) {
	bool result{ ImGui::Button(p_label.c_str()) };

	if (!p_tooltip_text.empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
		ImGui::SetTooltip(p_tooltip_text.c_str());

	return result;
}

std::optional<std::size_t> skc::imgui::spawn_schedule(const std::vector<bool>& p_schedule,
	const std::string& p_id_prefix) {
	std::optional<size_t> result;

	ImGui::Text("Spawn Schedule (initial)");

	for (std::size_t i{ 0 }; i < 32; ++i) {
		std::string l_id{ "###s01" + std::to_string(i) };
		bool l_sched_bit{ p_schedule.at(i) };
		if (ImGui::Checkbox(l_id.c_str(), &l_sched_bit))
			result = i;
		if (i % 8 != 7)
			ImGui::SameLine();
	}

	ImGui::NewLine();
	ImGui::Separator();
	ImGui::Text("Spawn Schedule (looping)");

	for (std::size_t i{ 32 }; i < 64; ++i) {
		bool l_sched_bit{ p_schedule.at(i) };
		std::string l_id{ "###" + p_id_prefix + std::to_string(i) };
		if (ImGui::Checkbox(l_id.c_str(), &l_sched_bit))
			result = i;
		if (i % 8 != 7)
			ImGui::SameLine();
	}

	return result;
}
