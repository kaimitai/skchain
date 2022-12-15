#ifndef SKC_GAME_METADATA_H
#define SKC_GAME_METADATA_H

#include <utility>
#include <vector>
#include <map>

using byte = unsigned char;
using position = std::pair<int, int>;

namespace skc {

	struct Game_metadata {

		std::map<std::size_t, position> m_meta_tiles;
		std::vector<std::vector<bool>> m_drop_schedules;
		std::vector<std::vector<byte>> m_drop_enemies;

	};

}

#endif
