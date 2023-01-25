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

// encoding
void skc::m66::patch_mirror_drop_schedule_bytes(std::vector<byte>& l_io_rom_data,
	const std::vector<std::vector<bool>>& p_drop_schedules,
	const std::vector<skc::Level>& p_levels
) {
	std::vector<std::vector<bool>> l_result_bits;

	for (const auto& lvl : p_levels) {
		for (std::size_t i{ 0 }; i < 2; ++i)
			l_result_bits.push_back(p_drop_schedules.at(lvl.get_spawn_schedule(i)));
	}

	auto l_result = klib::util::bitmask_to_bytes(l_result_bits);
	for (std::size_t i{ 0 }; i < l_result_bits.size(); ++i)
		l_io_rom_data.at(c::OFFSET_M66_DROP_SCHED_DATA + i) = l_result[i];
}

void skc::m66::patch_mirror_enemy_set_bytes(
	std::vector<byte>& l_io_rom_data,
	const std::vector<std::vector<byte>>& p_enemy_sets,
	const std::vector<skc::Level>& p_levels
) {

	for (std::size_t i{ 0 }; i < c::COUNT_M66_LEVELS; ++i) {
		for (std::size_t l_nmi_set_no{ 0 }; l_nmi_set_no < 2; ++l_nmi_set_no) {
			std::size_t l_out_offset{ c::OFFSET_M66_LVL_DATA + c::LENGTH_M66_LVL_DATA * i +
				(l_nmi_set_no == 0 ? c::OFFSET_M66_LOCAL_SCHED_ENEMY_1_DATA : c::OFFSET_M66_LOCAL_SCHED_ENEMY_2_DATA) };
			std::size_t l_enemy_set = p_levels.at(i).get_spawn_enemies(l_nmi_set_no);

			for (std::size_t nmi{ 0 }; nmi < 7 && nmi < p_enemy_sets.at(l_enemy_set).size(); ++nmi)
				l_io_rom_data.at(l_out_offset + nmi) = p_enemy_sets.at(l_enemy_set).at(nmi);

			l_io_rom_data.at(l_out_offset + p_enemy_sets.at(l_enemy_set).size()) = c::MIRROR_ENEMY_SET_DELIMITER;
		}
	}

}

void skc::m66::patch_enemy_data_bytes(std::vector<byte>& l_io_rom_data,
	const std::vector<skc::Level>& p_levels
) {
	for (std::size_t lvl_i{ 0 }; lvl_i < p_levels.size(); ++lvl_i) {
		auto l_enemy_data = p_levels[lvl_i].get_enemy_bytes();
		for (std::size_t i{ 0 }; i < l_enemy_data.size(); ++i)
			l_io_rom_data.at(c::OFFSET_M66_LVL_DATA
				+ lvl_i * c::LENGTH_M66_LVL_DATA + c::OFFSET_M66_LOCAL_ENEMY_DATA + i) = l_enemy_data[i];
	}
}

void skc::m66::patch_item_data_bytes(
	std::vector<byte>& l_io_rom_data,
	const std::vector<skc::Level>& p_levels
) {
	const auto set_block = [](std::vector<byte>& p_rom_data, std::size_t p_offset,
		std::pair<int, int> p_position, byte p_value) -> void {
			if (p_position.first < c::LEVEL_W && p_position.second >= 0 &&
				p_position.second < c::LEVEL_H) {
				p_rom_data.at(p_offset + p_position.first + c::LEVEL_W * p_position.second) = p_value;
			}
	};

	for (std::size_t i{ 0 }; i < p_levels.size(); ++i) {
		const auto& l_level{ p_levels[i] };
		std::size_t l_offset{ c::OFFSET_M66_LVL_DATA + c::LENGTH_M66_LVL_DATA * i };

		// patch map data
		for (int j{ 0 }; j < c::LEVEL_H; ++j)
			for (int i{ 0 }; i < c::LEVEL_W; ++i) {
				byte l_value{ 0xf8 };
				auto l_wall = l_level.get_wall_type(i, j);
				if (l_wall == skc::Wall::Brown)
					l_value = 0x90;
				else if (l_wall == skc::Wall::None)
					l_value = 0x10;
				set_block(l_io_rom_data, l_offset, std::make_pair(i, j), l_value);
			}

		// patch items
		const auto& l_items = l_level.get_items();
		for (const auto& l_item : l_items)
			set_block(l_io_rom_data, l_offset, l_item.get_position(), l_item.get_element_no());

		// patch metadata
		l_io_rom_data.at(l_offset + c::OFFSET_M66_KEY_STATUS) = l_level.get_key_status() +
			l_level.get_time_decrease_rate();
		l_io_rom_data.at(l_offset + c::OFFSET_M66_DOOR_POS) = skc::Level::get_byte_from_position(
			l_level.get_door_pos());
		l_io_rom_data.at(l_offset + c::OFFSET_M66_KEY_POS) = skc::Level::get_byte_from_position(
			l_level.get_key_pos());
		l_io_rom_data.at(l_offset + c::OFFSET_M66_PLAYER_START_POS) = skc::Level::get_byte_from_position(
			l_level.get_player_start_pos());
		l_io_rom_data.at(l_offset + c::OFFSET_M66_SPAWN01_POS) = skc::Level::get_byte_from_position(
			l_level.get_spawn_position(1));
		l_io_rom_data.at(l_offset + c::OFFSET_M66_SPAWN02_POS) = skc::Level::get_byte_from_position(
			l_level.get_spawn_position(0));

		if (l_level.has_constellation()) {
			l_io_rom_data.at(l_offset + c::OFFSET_M66_ITEM_DELIMITER) = l_level.get_constellation_no();
			l_io_rom_data.at(l_offset + c::OFFSET_M66_CONSTELLATION_POS) = skc::Level::get_byte_from_position(
				l_level.get_constellation_pos()
			);
		}
		else {
			l_io_rom_data.at(l_offset + c::OFFSET_M66_ITEM_DELIMITER) =
				c::ITEM_DELIMITER_MIN + 4 * l_level.get_tileset_no();
		}
	}

}
