#ifndef SKC_IMGUI_HELPER_H
#define SKC_IMGUI_HELPER_H

#include <string>
#include <utility>
#include <vector>
#include <optional>
#include "./../skc_constants/Constants_color.h"
#include "./../Enemy_editor.h"
#include "./../common/imgui/imgui.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"
#include "./../SKC_Config.h"
#include "./../SKC_Gfx.h"

using position = std::pair<int, int>;

namespace skc {
	namespace imgui {


		template<class T = int>
		std::optional<T> slider(const std::string& p_label,
			T p_value, T p_min, T p_max) {
			int l_value{ static_cast<int>(p_value) };
			if (ImGui::SliderInt(p_label.c_str(), &l_value, static_cast<int>(p_min), static_cast<int>(p_max)))
				return static_cast<T>(l_value);
			else
				return std::nullopt;
		}

		std::optional<bool> checkbox(const std::string& p_label, bool p_value);
		std::optional<position> position_sliders(const position& p_value, bool p_disabled = false);
		void tooltip_text(const std::string& p_text);
		void window(const std::string& p_title, int p_x, int p_y, int p_w, int p_h);
		bool button(const std::string& p_label,
			const std::size_t p_style_no = c::COLOR_STYLE_NORMAL,
			const std::string& p_tooltip = std::string());
		std::optional<std::size_t> spawn_schedule(const std::vector<bool>& p_schedule, const std::string& p_id_prefix);
		std::optional<byte> slider_enemy_properties(byte p_enemy_no, const skc::Enemy_editor& p_editor);

		void draw_drop_schedule_interface(const std::string& p_id, std::vector<std::vector<bool>>& p_schedule, std::size_t p_schedule_no);
		void draw_enemy_set_interface(std::vector<std::vector<byte>>& p_sets,
			const SKC_Config& p_config,
			const SKC_Gfx& p_gfx,
			byte p_available_monster_no,
			const Enemy_editor& p_editor,
			std::size_t p_enemy_set_no,
			std::size_t& p_sel_enemy_no);
	}
}

#endif
