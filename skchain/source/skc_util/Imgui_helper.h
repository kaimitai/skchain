#ifndef SKC_IMGUI_HELPER_H
#define SKC_IMGUI_HELPER_H

#include <string>
#include <utility>
#include <vector>
#include <optional>
#include "./../common/imgui/imgui.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"

using position = std::pair<int, int>;

namespace skc {
	namespace imgui {

		
		template<class T=int>
		std::optional<T> slider(const std::string& p_label,
			T p_value, T p_min, T p_max) {
			int l_value{ static_cast<int>(p_value) };
			if (ImGui::SliderInt(p_label.c_str(), &l_value, static_cast<int>(p_min), static_cast<int>(p_max)))
				return static_cast<T>(l_value);
			else
				return std::nullopt;
		}

		std::optional<bool> checkbox(const std::string& p_label, bool p_value);
		std::optional<position> position_sliders(const position& p_value);
		bool button(const std::string& p_label, const std::string& p_tooltip = std::string());
		std::optional<std::size_t> spawn_schedule(const std::vector<bool>& p_schedule, const std::string& p_id_prefix);
	}
}

#endif
