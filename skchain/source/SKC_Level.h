#ifndef SKC_LEVEL_H
#define SKC_LEVEL_H

#include <set>
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
		position get_position(void) const;
		Element_type get_element_type(void) const;
		byte get_element_no(void) const;
	};

	class Level {
		std::vector<std::vector<skc::Wall>> m_tiles;
		std::vector<Level_element> m_elements;
		std::vector<byte> m_item_header;
		std::optional<Level_element> m_constellation;
		std::optional<byte> m_item_eof;

		void add_blocks(byte p_b1, byte p_b2, byte p_w1, byte p_w2);
		byte m_key_status, m_spawn_rate, m_spawn01, m_spawn02;
		std::pair<int, int> m_fixed_start_pos, m_fixed_key_pos, m_fixed_door_pos;

		std::vector<std::size_t> get_item_indexes(byte p_item_no, std::set<std::size_t>& p_ignored_indexes) const;

		static std::pair<int, int> get_position_from_byte(byte b);
		static byte get_byte_from_position(const std::pair<int, int>& p_position);
		static bool is_item_delimiter(byte p_value);

	public:
		Level(void);
		void load_block_data(const std::vector<byte>& p_bytes, std::size_t p_offset);
		void load_item_data(const std::vector<byte>& p_bytes, std::size_t p_offset);
		void load_enemy_data(const std::vector<byte>& p_bytes, std::size_t p_offset);
		skc::Wall get_wall_type(int p_x, int p_y) const;

		position get_player_start_pos(void) const;
		position get_door_pos(void) const;
		position get_key_pos(void) const;
		byte get_key_status(void) const;
		bool has_constellation(void) const;
		byte get_constellation_no(void) const;
		position get_constellation_pos(void) const;
		byte get_spawn_rate(void) const;
		byte get_spawn01(void) const;
		byte get_spawn02(void) const;
		const std::vector<byte>& get_item_header(void) const;
		bool has_item_eof(void) const;
		byte get_item_eof(void) const;

		const std::vector<skc::Level_element>& get_elements(void) const;

		void set_player_start_pos(int p_x, int p_y);

		static bool is_item_constellation(byte p_item_no);

		// logical getters
		std::vector<byte> get_item_bytes(void) const;
		std::vector<byte> get_enemy_bytes(void) const;
	};

}

#endif
