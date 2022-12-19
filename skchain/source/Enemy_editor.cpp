#include "Enemy_editor.h"

#include <stdexcept>

skc::Enemy_editor::Enemy_editor(void) {
	m_direction_bundles = {
		// bullets
		{c::E_BULLET_RIGHT, c::E_BULLET_LEFT, c::E_BULLET_UP, c::E_BULLET_DOWN},

		// panel monsters
		{c::E_PANEL_RIGHT, c::E_PANEL_LEFT, c::E_PANEL_UP, c::E_PANEL_DOWN},

		// fireball sets
		{c::E_FB_S0_RIGHT, c::E_FB_S0_LEFT, c::E_FB_S0_UP, c::E_FB_S0_DOWN},
		{c::E_FB_S1_RIGHT, c::E_FB_S1_LEFT, c::E_FB_S1_UP, c::E_FB_S1_DOWN},

		// neul sets
		{c::E_NEUL_S0_UP_1, c::E_NEUL_S0_DOWN_1},
		{c::E_NEUL_S0_UP_2, c::E_NEUL_S0_DOWN_2},
		{c::E_NEUL_S1_UP_1, c::E_NEUL_S1_DOWN_1},
		{c::E_NEUL_S1_UP_2, c::E_NEUL_S1_DOWN_2},
		{c::E_NEUL_S2_UP_1, c::E_NEUL_S2_DOWN_1},
		{c::E_NEUL_S2_UP_2, c::E_NEUL_S2_DOWN_2},
		{c::E_NEUL_S3_UP_1, c::E_NEUL_S3_DOWN_1},
		{c::E_NEUL_S3_UP_2, c::E_NEUL_S3_DOWN_2},

		// ghost sets
		{c::E_GHOST_S0_RIGHT_1, c::E_GHOST_S0_LEFT_1},
		{c::E_GHOST_S0_RIGHT_2, c::E_GHOST_S0_LEFT_2},
		{c::E_GHOST_S1_RIGHT_1, c::E_GHOST_S1_LEFT_1},
		{c::E_GHOST_S1_RIGHT_2, c::E_GHOST_S1_LEFT_2},
		{c::E_GHOST_S2_RIGHT_1, c::E_GHOST_S2_LEFT_1},
		{c::E_GHOST_S2_RIGHT_2, c::E_GHOST_S2_LEFT_2},
		{c::E_GHOST_S3_RIGHT_1, c::E_GHOST_S3_LEFT_1},
		{c::E_GHOST_S3_RIGHT_2, c::E_GHOST_S3_LEFT_2},

		// demonhead sets
		{c::E_DH_S0_RIGHT_1, c::E_DH_S0_LEFT_1},
		{c::E_DH_S0_RIGHT_2, c::E_DH_S0_LEFT_2},
		{c::E_DH_S1_RIGHT_1, c::E_DH_S1_LEFT_1},
		{c::E_DH_S1_RIGHT_2, c::E_DH_S1_LEFT_2},
		{c::E_DH_S2_RIGHT_1, c::E_DH_S2_LEFT_1},
		{c::E_DH_S2_RIGHT_2, c::E_DH_S2_LEFT_2},

		// saramandor sets
		{c::E_SARA_S0_RIGHT_1, c::E_SARA_S0_LEFT_1},
		{c::E_SARA_S0_RIGHT_2, c::E_SARA_S0_LEFT_2},
		{c::E_SARA_S1_RIGHT_1, c::E_SARA_S1_LEFT_1},
		{c::E_SARA_S1_RIGHT_2, c::E_SARA_S1_LEFT_2},
		{c::E_SARA_S2_RIGHT_1, c::E_SARA_S2_LEFT_1},
		{c::E_SARA_S2_RIGHT_2, c::E_SARA_S2_LEFT_2},

		// dragon sets
		{c::E_DRAGON_S0_LEFT_1, c::E_DRAGON_S0_RIGHT_1},
		{c::E_DRAGON_S0_LEFT_2, c::E_DRAGON_S0_RIGHT_2},
		{c::E_DRAGON_S1_LEFT_1, c::E_DRAGON_S1_RIGHT_1},
		{c::E_DRAGON_S1_LEFT_2, c::E_DRAGON_S1_RIGHT_2},

		// golem sets
		{c::E_GOLEM_S0_LEFT_1, c::E_GOLEM_S0_RIGHT_1},
		{c::E_GOLEM_S0_LEFT_2, c::E_GOLEM_S0_RIGHT_2},
		{c::E_GOLEM_S1_LEFT_1, c::E_GOLEM_S1_RIGHT_1},
		{c::E_GOLEM_S1_LEFT_2, c::E_GOLEM_S1_RIGHT_2},

		// gargoyle sets
		{c::E_GARGOYLE_S0_LEFT_1, c::E_GARGOYLE_S0_RIGHT_1},
		{c::E_GARGOYLE_S0_LEFT_2, c::E_GARGOYLE_S0_RIGHT_2},
		{c::E_GARGOYLE_S1_LEFT_1, c::E_GARGOYLE_S1_RIGHT_1},
		{c::E_GARGOYLE_S1_LEFT_2, c::E_GARGOYLE_S1_RIGHT_2}
	};

	m_speed_bundles = {
		// fireball sets
		{c::E_FB_S0_RIGHT, c::E_FB_S1_RIGHT},
		{c::E_FB_S0_LEFT, c::E_FB_S1_LEFT},
		{c::E_FB_S0_UP, c::E_FB_S1_UP},
		{c::E_FB_S0_DOWN, c::E_FB_S1_DOWN},

		// neul sets
		{c::E_NEUL_S0_DOWN_1, c::E_NEUL_S1_DOWN_1, c::E_NEUL_S2_DOWN_1, c::E_NEUL_S3_DOWN_1},
		{c::E_NEUL_S0_DOWN_2, c::E_NEUL_S1_DOWN_2, c::E_NEUL_S2_DOWN_2, c::E_NEUL_S3_DOWN_2},
		{c::E_NEUL_S0_UP_1, c::E_NEUL_S1_UP_1, c::E_NEUL_S2_UP_1, c::E_NEUL_S3_UP_1},
		{c::E_NEUL_S0_UP_2, c::E_NEUL_S1_UP_2, c::E_NEUL_S2_UP_2, c::E_NEUL_S3_UP_2},

		// ghost sets
		{c::E_GHOST_S0_RIGHT_1, c::E_GHOST_S1_RIGHT_1, c::E_GHOST_S2_RIGHT_1, c::E_GHOST_S3_RIGHT_1},
		{c::E_GHOST_S0_RIGHT_2, c::E_GHOST_S1_RIGHT_2, c::E_GHOST_S2_RIGHT_2, c::E_GHOST_S3_RIGHT_2},
		{c::E_GHOST_S0_LEFT_1, c::E_GHOST_S1_LEFT_1, c::E_GHOST_S2_LEFT_1, c::E_GHOST_S3_LEFT_1},
		{c::E_GHOST_S0_LEFT_2, c::E_GHOST_S1_LEFT_2, c::E_GHOST_S2_LEFT_2, c::E_GHOST_S3_LEFT_2},

		// demonhead sets
		{c::E_DH_S0_RIGHT_1, c::E_DH_S1_RIGHT_1, c::E_DH_S2_RIGHT_1},
		{c::E_DH_S0_RIGHT_2, c::E_DH_S1_RIGHT_2, c::E_DH_S2_RIGHT_2},
		{c::E_DH_S0_LEFT_1, c::E_DH_S1_LEFT_1, c::E_DH_S2_LEFT_1},
		{c::E_DH_S0_LEFT_2, c::E_DH_S1_LEFT_2, c::E_DH_S2_LEFT_2},

		// saramandor sets
		{c::E_SARA_S0_RIGHT_1, c::E_SARA_S1_RIGHT_1, c::E_SARA_S2_RIGHT_1},
		{c::E_SARA_S0_RIGHT_2, c::E_SARA_S1_RIGHT_2, c::E_SARA_S2_RIGHT_2},
		{c::E_SARA_S0_LEFT_1, c::E_SARA_S1_LEFT_1, c::E_SARA_S2_LEFT_1},
		{c::E_SARA_S0_LEFT_2, c::E_SARA_S1_LEFT_2, c::E_SARA_S2_LEFT_2},

		// dragon sets
		{c::E_DRAGON_S0_LEFT_1, c::E_DRAGON_S1_LEFT_1},
		{c::E_DRAGON_S0_LEFT_2, c::E_DRAGON_S1_LEFT_2},
		{c::E_DRAGON_S0_RIGHT_1, c::E_DRAGON_S1_RIGHT_1},
		{c::E_DRAGON_S0_RIGHT_2, c::E_DRAGON_S1_RIGHT_2},

		// golem sets
		{c::E_GOLEM_S0_LEFT_1, c::E_GOLEM_S1_LEFT_1},
		{c::E_GOLEM_S0_LEFT_2, c::E_GOLEM_S1_LEFT_2},
		{c::E_GOLEM_S0_RIGHT_1, c::E_GOLEM_S1_RIGHT_1},
		{c::E_GOLEM_S0_RIGHT_2, c::E_GOLEM_S1_RIGHT_2},

		// gargoyle sets
		{c::E_GARGOYLE_S0_LEFT_1, c::E_GARGOYLE_S1_LEFT_1},
		{c::E_GARGOYLE_S0_LEFT_2, c::E_GARGOYLE_S1_LEFT_2},
		{c::E_GARGOYLE_S0_RIGHT_1, c::E_GARGOYLE_S1_RIGHT_1},
		{c::E_GARGOYLE_S0_RIGHT_2, c::E_GARGOYLE_S1_RIGHT_2}
	};

	for (std::size_t i{ 0 }; i < m_direction_bundles.size(); ++i)
		for (byte l_enemy_no : m_direction_bundles[i])
			m_db_map.insert(std::make_pair(l_enemy_no, i));

	for (std::size_t i{ 0 }; i < m_speed_bundles.size(); ++i)
		for (byte l_enemy_no : m_speed_bundles[i])
			m_sb_map.insert(std::make_pair(l_enemy_no, i));
}

bool skc::Enemy_editor::has_direction(byte p_enemy_no) const {
	return m_db_map.find(p_enemy_no) != end(m_db_map);
}

bool skc::Enemy_editor::has_speed(byte p_enemy_no) const {
	return m_sb_map.find(p_enemy_no) != end(m_sb_map);
}

std::size_t skc::Enemy_editor::get_direction_size(byte p_enemy_no) const {
	return m_direction_bundles[m_db_map.at(p_enemy_no)].size();
}

std::size_t skc::Enemy_editor::get_speed_size(byte p_enemy_no) const {
	return m_speed_bundles[m_sb_map.at(p_enemy_no)].size();
}

byte skc::Enemy_editor::get_enemy_no_by_speed(byte p_enemy_no, std::size_t p_speed) const {
	return get_enemy_no(p_speed, m_speed_bundles[m_sb_map.at(p_enemy_no)]);
}

byte skc::Enemy_editor::get_enemy_no_by_direction(byte p_enemy_no, std::size_t p_direction) const {
	return get_enemy_no(p_direction, m_direction_bundles[m_db_map.at(p_enemy_no)]);
}

std::size_t skc::Enemy_editor::get_direction(byte p_enemy_no) const {
	return get_enemy_property(p_enemy_no, m_direction_bundles[m_db_map.at(p_enemy_no)]);
}

std::size_t skc::Enemy_editor::get_speed(byte p_enemy_no) const {
	return get_enemy_property(p_enemy_no, m_speed_bundles[m_sb_map.at(p_enemy_no)]);
}

std::size_t skc::Enemy_editor::get_enemy_property(byte p_enemy_no,
	const std::vector<byte>& p_bundle) const {

	for (std::size_t i{ 0 }; i < p_bundle.size(); ++i)
		if (p_bundle[i] == p_enemy_no)
			return i;

	throw std::runtime_error("Enemy property misconfiguration");

}

byte skc::Enemy_editor::get_enemy_no(std::size_t p_property_no,
	const std::vector<byte>& p_bundle) const {
	return p_bundle[p_property_no];

	throw std::runtime_error("Enemy property misconfiguration");
}
