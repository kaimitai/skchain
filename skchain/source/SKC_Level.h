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
		byte get_item_no(void) const;

		void set_item_hidden(bool p_value);
		void set_item_in_block(bool p_value);
		void set_position(const position& p_pos);
	};

	struct Demon_mirror {
		byte m_schedule_no, m_monster_set_no;
		position m_position;
		Demon_mirror(const position& p_position, byte p_schedule_no, byte p_monster_set_no);
	};

	class Level {
		std::vector<std::vector<skc::Wall>> m_tiles;
		std::vector<Level_element> m_items, m_enemies;
		std::optional<Level_element> m_constellation;
		std::vector<Demon_mirror> m_demon_mirrors;

		byte m_key_status, m_time_decrease_rate, m_spawn_enemy_lifetime, m_tileset_no;
		std::pair<int, int> m_fixed_start_pos, m_fixed_key_pos, m_fixed_door_pos;

		std::vector<std::size_t> get_item_indexes(byte p_item_no, std::set<std::size_t>& p_ignored_indexes) const;

		static skc::Wall walls_to_wall_type(bool p_bblock, bool p_wblock);
		static bool is_item_delimiter(byte p_value);

	public:
		static std::pair<int, int> get_position_from_byte(byte b);
		static byte get_byte_from_position(const std::pair<int, int>& p_position);

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
		byte get_spawn_enemy_lifetime(void) const;
		byte get_time_decrease_rate(void) const;
		byte get_tileset_no(void) const;
		byte get_item_delimiter(void) const;

		void set_spawn_enemy_lifetime(byte p_value);
		void set_time_decrease_rate(byte p_value);

		const std::vector<skc::Level_element>& get_enemies(void) const;
		const std::vector<skc::Level_element>& get_items(void) const;

		void set_player_start_pos(const std::pair<int, int>& p_pos);
		void set_key_pos(const std::pair<int, int>& p_pos);
		void set_door_pos(const std::pair<int, int>& p_pos);
		void set_constellation(byte p_constellation_no, const std::pair<int, int>& p_pos);
		void set_block(skc::Wall p_wall_type, const std::pair<int, int>& p_pos);
		void set_blocks(const std::vector<std::vector<skc::Wall>>& p_blocks);
		void set_tileset_no(byte p_tileset_no);
		void add_item(byte p_item_no, const position& p_pos);
		void delete_item(int p_index);
		void add_enemy(byte p_enemy_no, const position& p_pos);
		void delete_enemy(int p_index);
		void delete_constellation(void);

		position get_spawn_position(std::size_t p_index) const;
		byte get_spawn_schedule(std::size_t p_index) const;
		byte get_spawn_enemies(std::size_t p_index) const;

		void set_spawn_position(std::size_t p_index, const position& p_pos);
		void set_spawn_schedule(std::size_t p_index, byte p_value);
		void set_spawn_enemies(std::size_t p_index, byte p_value);

		void set_item_hidden(int p_index, bool p_value);
		void set_item_in_block(int p_index, bool p_value);
		void set_item_position(int p_index, const position& l_pos);

		void set_enemy_position(int p_index, const position& l_pos);

		static bool is_item_constellation(byte p_item_no);
		static bool is_item_in_block(byte p_item_no);
		static bool is_item_hidden(byte p_item_no);

		// logical getters
		int get_item_count(void) const;
		int get_enemy_count(void) const;

		int get_item_index(const position& p_pos) const;
		int get_enemy_index(const position& p_pos) const;

		bool has_item_at_position(const std::pair<int, int>& p_pos) const;
		bool has_enemy_at_position(const std::pair<int, int>& p_pos) const;

		std::vector<byte> get_block_bytes(void) const;
		std::vector<byte> get_item_bytes(const std::vector<byte>& p_ignore_item_elements) const;
		std::vector<byte> get_enemy_bytes(void) const;
		std::vector<byte> get_item_bitmask_bytes(byte p_item_element_no) const;

		bool is_key_hidden(void) const;
		bool is_key_removed(void) const;
		bool is_key_in_block(void) const;

		void set_key_hidden(bool p_value);
		void set_key_in_block(bool p_value);
		void set_key_removed(void);
		void set_key_status(byte p_value);

		bool is_door_removed(void) const;
		void set_door_removed(void);
	};

}

#endif
