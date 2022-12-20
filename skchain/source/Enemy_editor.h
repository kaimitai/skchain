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
		Enemy_editor(void) = default;
		void generate_maps(void);
		void add_speed_bundle(const std::vector<byte>& p_enemy_list);
		void add_direction_bundle(const std::vector<byte>& p_enemy_list);

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

		// neul speed 1
		constexpr byte E_NEUL_S1_UP_1{ 0x38 };
		constexpr byte E_NEUL_S1_UP_2{ 0x39 };
		constexpr byte E_NEUL_S1_DOWN_1{ 0x3a };
		constexpr byte E_NEUL_S1_DOWN_2{ 0x3b };

		// neul speed 2
		constexpr byte E_NEUL_S2_UP_1{ 0x40 };
		constexpr byte E_NEUL_S2_UP_2{ 0x41 };
		constexpr byte E_NEUL_S2_DOWN_1{ 0x42 };
		constexpr byte E_NEUL_S2_DOWN_2{ 0x43 };

		// neul speed 3
		constexpr byte E_NEUL_S3_UP_1{ 0x48 };
		constexpr byte E_NEUL_S3_UP_2{ 0x49 };
		constexpr byte E_NEUL_S3_DOWN_1{ 0x4a };
		constexpr byte E_NEUL_S3_DOWN_2{ 0x4b };

		// ghosts speed 0
		constexpr byte E_GHOST_S0_RIGHT_1{ 0x34 };
		constexpr byte E_GHOST_S0_RIGHT_2{ 0x35 };
		constexpr byte E_GHOST_S0_LEFT_1{ 0x36 };
		constexpr byte E_GHOST_S0_LEFT_2{ 0x37 };

		// ghosts speed 1
		constexpr byte E_GHOST_S1_RIGHT_1{ 0x3c };
		constexpr byte E_GHOST_S1_RIGHT_2{ 0x3d };
		constexpr byte E_GHOST_S1_LEFT_1{ 0x3e };
		constexpr byte E_GHOST_S1_LEFT_2{ 0x3f };

		// ghosts speed 2
		constexpr byte E_GHOST_S2_RIGHT_1{ 0x44 };
		constexpr byte E_GHOST_S2_RIGHT_2{ 0x45 };
		constexpr byte E_GHOST_S2_LEFT_1{ 0x46 };
		constexpr byte E_GHOST_S2_LEFT_2{ 0x47 };

		// ghosts speed 3
		constexpr byte E_GHOST_S3_RIGHT_1{ 0x4c };
		constexpr byte E_GHOST_S3_RIGHT_2{ 0x4d };
		constexpr byte E_GHOST_S3_LEFT_1{ 0x4e };
		constexpr byte E_GHOST_S3_LEFT_2{ 0x4f };

		// demonheads speed 0
		constexpr byte E_DH_S0_RIGHT_1{ 0x50 };
		constexpr byte E_DH_S0_LEFT_1{ 0x51 };
		constexpr byte E_DH_S0_RIGHT_2{ 0x52 };
		constexpr byte E_DH_S0_LEFT_2{ 0x53 };

		// demonheads speed 1
		constexpr byte E_DH_S1_RIGHT_1{ 0x54 };
		constexpr byte E_DH_S1_LEFT_1{ 0x55 };
		constexpr byte E_DH_S1_RIGHT_2{ 0x56 };
		constexpr byte E_DH_S1_LEFT_2{ 0x57 };

		// demonheads speed 2
		constexpr byte E_DH_S2_RIGHT_1{ 0x58 };
		constexpr byte E_DH_S2_LEFT_1{ 0x59 };
		constexpr byte E_DH_S2_RIGHT_2{ 0x5a };
		constexpr byte E_DH_S2_LEFT_2{ 0x5b };

		// saramandhors speed 0
		constexpr byte E_SARA_S0_RIGHT_1{ 0x5c };
		constexpr byte E_SARA_S0_LEFT_1{ 0x5d };
		constexpr byte E_SARA_S0_RIGHT_2{ 0x5e };
		constexpr byte E_SARA_S0_LEFT_2{ 0x5f };

		// saramandhors speed 1
		constexpr byte E_SARA_S1_RIGHT_1{ 0x60 };
		constexpr byte E_SARA_S1_LEFT_1{ 0x61 };
		constexpr byte E_SARA_S1_RIGHT_2{ 0x62 };
		constexpr byte E_SARA_S1_LEFT_2{ 0x63 };

		// saramandhors speed 2
		constexpr byte E_SARA_S2_RIGHT_1{ 0x64 };
		constexpr byte E_SARA_S2_LEFT_1{ 0x65 };
		constexpr byte E_SARA_S2_RIGHT_2{ 0x66 };
		constexpr byte E_SARA_S2_LEFT_2{ 0x67 };

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

		// flames
		constexpr byte E_FLAME_RED_1{ 0x80 };
		constexpr byte E_FLAME_WHITE_1{ 0x81 };
		constexpr byte E_FLAME_RED_2{ 0x82 };
		constexpr byte E_FLAME_WHITE_2{ 0x83 };

		// texts
		constexpr char TXT_ERR_MISCONFIG[]{ "Enemy property misconfiguration" };
	}

}

#endif
