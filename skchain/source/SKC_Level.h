#ifndef SKC_LEVEL_H
#define SKC_LEVEL_H

#include <string>
#include <vector>

using byte = unsigned char;

namespace skc {

	enum class Wall { None, Brown, White, Brown_white };

	class Level {
		std::vector<std::vector<skc::Wall>> m_tiles;

		void add_blocks(byte p_b1, byte p_b2, byte p_w1, byte p_w2);

	public:
		Level(const std::vector<byte>& p_browns, const std::vector<byte>& p_whites);

		// TODO: Remove debug functions
		std::string get_blocks(void) const;
	};

}

#endif
