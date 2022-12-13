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
