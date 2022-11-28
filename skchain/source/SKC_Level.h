#ifndef SKC_LEVEL_H
#define SKC_LEVEL_H

#include <string>
#include <utility>
#include <vector>

using byte = unsigned char;

namespace skc {

	enum class Wall { None, Brown, White, Brown_white };

	class Level {
		std::vector<std::vector<skc::Wall>> m_tiles;

		void add_blocks(byte p_b1, byte p_b2, byte p_w1, byte p_w2);
		std::pair<byte, byte> m_fixed_start_pos, m_fixed_key_pos, m_fixed_door_pos;

	public:
		Level(const std::vector<byte>& p_browns, const std::vector<byte>& p_whites);
		skc::Wall get_wall_type(int p_x, int p_y) const;

		std::pair<byte, byte> get_player_start_pos(void) const;

		void set_player_start_pos(byte p_x, byte p_y);

		static bool is_item_constellation(byte p_item_no);

		// TODO: Remove debug functions
		std::string get_blocks(void) const;
	};

}

#endif
