#include "Imgui_helper.h"
#include "./../common/imgui/imgui.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"

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
