#include <utility>
#include "Rom_expander.h"
#include "./../common/klib/klib_util.h"
#include "./../skc_constants/Constants_level.h"

skc::Level skc::m66::parse_level(const std::vector<byte>& p_rom_data,
	std::size_t p_level_no) {
	skc::Level result;

	// parse the map (blocks and items)
	std::size_t l_offset{ c::OFFSET_M66_LVL_DATA + 256 * p_level_no };

	for (std::size_t j{ 0 }; j < c::LEVEL_H; ++j)
		for (std::size_t i{ 0 }; i < c::LEVEL_W; ++i) {
			byte l_value{ p_rom_data.at(l_offset + j * c::LEVEL_W + i) };
			std::pair<int, int> l_pos{ std::make_pair(static_cast<int>(i), static_cast<int>(j)) };

			if (l_value == 0xf8)
				result.set_block(skc::Wall::White, l_pos);
			else if (l_value == 0x90)
				result.set_block(skc::Wall::Brown, l_pos);
			else if (l_value != 0x10)
				result.add_item(l_value, l_pos);
		}

	// parse enemy data
	result.load_enemy_data(p_rom_data, l_offset + c::OFFSET_M66_LOCAL_ENEMY_DATA);

	// parse metadata
	result.set_key_status_and_time_dr(p_rom_data.at(l_offset + c::OFFSET_M66_KEY_STATUS));
	result.set_door_pos(skc::Level::get_position_from_byte(p_rom_data.at(l_offset + c::OFFSET_M66_DOOR_POS)));
	result.set_key_pos(skc::Level::get_position_from_byte(p_rom_data.at(l_offset + c::OFFSET_M66_KEY_POS)));
	result.set_player_start_pos(skc::Level::get_position_from_byte(p_rom_data.at(l_offset + c::OFFSET_M66_PLAYER_START_POS)));
	result.set_spawn_position(1, skc::Level::get_position_from_byte(p_rom_data.at(l_offset + c::OFFSET_M66_SPAWN01_POS)));
	result.set_spawn_position(0, skc::Level::get_position_from_byte(p_rom_data.at(l_offset + c::OFFSET_M66_SPAWN02_POS)));

	byte l_item_delimiter{ p_rom_data.at(l_offset + c::OFFSET_M66_ITEM_DELIMITER) };
	result.set_tileset_no(l_item_delimiter >> 2 & 3);

	if (l_item_delimiter >= c::ITEM_CONSTELLATION_MIN)
		result.set_constellation(l_item_delimiter,
			skc::Level::get_position_from_byte(p_rom_data.at(l_offset + c::OFFSET_M66_CONSTELLATION_POS)));

	return result;
}

std::vector<std::vector<byte>> skc::m66::parse_mirror_enemy_sets(const std::vector<byte>& p_rom_data) {
	std::vector<std::vector<byte>> result;

	for (std::size_t i{ 0 }; i < c::COUNT_M66_LEVELS; ++i) {
		for (std::size_t l_nmi_set_no{ 0 }; l_nmi_set_no < 2; ++l_nmi_set_no) {
			std::vector<byte> l_enemy_set;
			std::size_t l_offset{ c::OFFSET_M66_LVL_DATA + c::LENGTH_M66_LVL_DATA * i +
				(l_nmi_set_no == 0 ? c::OFFSET_M66_LOCAL_SCHED_ENEMY_1_DATA : c::OFFSET_M66_LOCAL_SCHED_ENEMY_2_DATA) };
			for (std::size_t i{ 0 }; i < 8 && p_rom_data.at(l_offset + i) != c::MIRROR_ENEMY_SET_DELIMITER; ++i)
				l_enemy_set.push_back(p_rom_data.at(l_offset + i));

			result.push_back(l_enemy_set);
		}
	}

	return result;
}

std::vector<std::vector<bool>> skc::m66::parse_mirror_drop_schedules(const std::vector<byte>& p_rom_data) {
	return klib::util::bytes_to_bitmask(p_rom_data, 64, 53 * 2, c::OFFSET_M66_DROP_SCHED_DATA);
}
