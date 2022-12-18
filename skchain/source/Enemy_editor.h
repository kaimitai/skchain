#ifndef SKC_ENEMY_EDITOR_H
#define SKC_ENEMY_EDITOR_H

#include <map>
#include <vector>

using byte = unsigned char;

namespace skc {

	class Enemy_editor {

		std::vector<std::vector<byte>> m_speed_bundles, m_direction_bundles;
		std::map<byte, std::size_t> m_sb_map, m_db_map;

		std::size_t get_enemy_property(byte p_enemy_no,
			const std::vector<byte>& p_bundle) const;
		byte get_enemy_no(std::size_t p_property_no,
			const std::vector<byte>& p_bundle) const;

	public:
		Enemy_editor(void);

		bool has_direction(byte p_enemy_no) const;
		bool has_speed(byte p_enemy_no) const;
		std::size_t get_direction_size(byte p_enemy_no) const;
		std::size_t get_speed_size(byte p_enemy_no) const;

		std::size_t get_direction(byte p_enemy_no) const;
		std::size_t get_speed(byte p_enemy_no) const;

		byte get_enemy_no_by_speed(byte p_enemy_no, std::size_t p_speed) const;
		byte get_enemy_no_by_direction(byte p_enemy_no, std::size_t p_direction) const;
	};

	// enemy number constants

	namespace c {

		// bullets
		constexpr byte E_BULLET_RIGHT{ 0x20 };
		constexpr byte E_BULLET_LEFT{ 0x21 };
		constexpr byte E_BULLET_UP{ 0x22 };
		constexpr byte E_BULLET_DOWN{ 0x23 };

		// panel monsters
		constexpr byte E_PANEL_RIGHT{ 0x24 };
		constexpr byte E_PANEL_LEFT{ 0x25 };
		constexpr byte E_PANEL_UP{ 0x26 };
		constexpr byte E_PANEL_DOWN{ 0x27 };

		// fireballs speed 0
		constexpr byte E_FB_S0_RIGHT{ 0x28 };
		constexpr byte E_FB_S0_LEFT{ 0x29 };
		constexpr byte E_FB_S0_UP{ 0x2a };
		constexpr byte E_FB_S0_DOWN{ 0x2b };

		// fireballs speed 1
		constexpr byte E_FB_S1_RIGHT{ 0x2c };
		constexpr byte E_FB_S1_LEFT{ 0x2d };
		constexpr byte E_FB_S1_UP{ 0x2e };
		constexpr byte E_FB_S1_DOWN{ 0x2f };

		// neul speed 0
		constexpr byte E_NEUL_S0_UP_1{ 0x30 };
		constexpr byte E_NEUL_S0_UP_2{ 0x31 };
		constexpr byte E_NEUL_S0_DOWN_1{ 0x32 };
		constexpr byte E_NEUL_S0_DOWN_2{ 0x33 };

		// dragons speed 0
		constexpr byte E_DRAGON_S0_RIGHT_1{ 0x68 };
		constexpr byte E_DRAGON_S0_LEFT_1{ 0x69 };
		constexpr byte E_DRAGON_S0_RIGHT_2{ 0x6a };
		constexpr byte E_DRAGON_S0_LEFT_2{ 0x6b };

		// dragons speed 1
		constexpr byte E_DRAGON_S1_RIGHT_1{ 0x6c };
		constexpr byte E_DRAGON_S1_LEFT_1{ 0x6d };
		constexpr byte E_DRAGON_S1_RIGHT_2{ 0x6e };
		constexpr byte E_DRAGON_S1_LEFT_2{ 0x6f };

		// golems speed 0
		constexpr byte E_GOLEM_S0_RIGHT_1{ 0x70 };
		constexpr byte E_GOLEM_S0_LEFT_1{ 0x71 };
		constexpr byte E_GOLEM_S0_RIGHT_2{ 0x72 };
		constexpr byte E_GOLEM_S0_LEFT_2{ 0x73 };

		// golems speed 1
		constexpr byte E_GOLEM_S1_RIGHT_1{ 0x74 };
		constexpr byte E_GOLEM_S1_LEFT_1{ 0x75 };
		constexpr byte E_GOLEM_S1_RIGHT_2{ 0x76 };
		constexpr byte E_GOLEM_S1_LEFT_2{ 0x77 };

		// gargoyles speed 0
		constexpr byte E_GARGOYLE_S0_RIGHT_1{ 0x78 };
		constexpr byte E_GARGOYLE_S0_LEFT_1{ 0x79 };
		constexpr byte E_GARGOYLE_S0_RIGHT_2{ 0x7a };
		constexpr byte E_GARGOYLE_S0_LEFT_2{ 0x7b };

		// gargoyles speed 1
		constexpr byte E_GARGOYLE_S1_RIGHT_1{ 0x7c };
		constexpr byte E_GARGOYLE_S1_LEFT_1{ 0x7d };
		constexpr byte E_GARGOYLE_S1_RIGHT_2{ 0x7e };
		constexpr byte E_GARGOYLE_S1_LEFT_2{ 0x7f };
	}

}

#endif
