#ifndef SKC_ENEMY_SET_EDITOR_H
#define SKC_ENEMY_SET_EDITOR_H

#include "SKC_Config.h"
#include "SKC_Gfx.h"
#include "Enemy_editor.h"
#include <vector>

using byte = unsigned char;

namespace skc {

	class Enemy_set_editor {
		std::size_t m_current_set_index, m_current_monster_index;

	public:
		void draw_ui(std::vector<std::vector<byte>>& p_sets,
			const SKC_Config& p_config,
			const SKC_Gfx& p_gfx,
			byte p_available_monster_no,
			const Enemy_editor& p_editor);
	};

}

#endif
