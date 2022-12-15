#ifndef SKC_IMGUI_HELPER_H
#define SKC_IMGUI_HELPER_H

#include <string>
#include <utility>
#include <optional>

using position = std::pair<int, int>;

namespace skc {
	namespace imgui {

		std::optional<int> slider(const std::string& p_label,
			int p_value, int p_min, int p_max);
		std::optional<bool> checkbox(const std::string& p_label, bool p_value);
		std::optional<position> position_sliders(const position& p_value);
		bool button(const std::string& p_label, const std::string& p_tooltip = std::string());
	}
}

#endif
