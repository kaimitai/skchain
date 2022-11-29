#ifndef SKC_LEVEL_H
#define SKC_LEVEL_H

#include <string>
#include <utility>
#include <vector>
#include <optional>

using byte = unsigned char;
using position = std::pair<int, int>;

namespace skc {

	enum class Wall { None, Brown, White, Brown_white };
	enum class Element_type { Item, Enemy };

	class Level_element {
		skc::Element_type m_type;
		position m_position;
		byte m_element_no;

	public:
		Level_element(skc::Element_type p_type, position p_position, byte p_element_no);
	};

	class Level {
		std::vector<std::vector<skc::Wall>> m_tiles;
		std::vector<Level_element> m_elements;
		std::vector<byte> m_item_header;
		std::optional<Level_element> m_constellation;

		void add_blocks(byte p_b1, byte p_b2, byte p_w1, byte p_w2);
		byte m_key_status, m_spawn_rate;
		std::pair<int, int> m_fixed_start_pos, m_fixed_key_pos, m_fixed_door_pos;

		static std::pair<int, int> get_position_from_byte(byte b);

	public:
		Level(void);
		void load_block_data(const std::vector<byte>& p_bytes, std::size_t p_offset);
		void load_item_data(const std::vector<byte>& p_bytes, std::size_t p_offset);
		void load_enemy_data(const std::vector<byte>& p_bytes, std::size_t p_offset);
		skc::Wall get_wall_type(int p_x, int p_y) const;

		std::pair<int, int> get_player_start_pos(void) const;

		void set_player_start_pos(int p_x, int p_y);

		static bool is_item_constellation(byte p_item_no);

		// TODO: Remove debug functions
		std::string get_blocks(void) const;
	};

}

#endif
