#include <algorithm>
#include <stdexcept>
#include <utility>
#include "Rom_expander.h"
#include "./../common/klib/klib_util.h"
#include "./../skc_constants/Constants_application.h"
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

	// cleanup any demon mirrors which we do not need to see in the editor
	// these are demon mirror items on top of a visible demon mirror metadata location
	mirror_item_cleanup(result);

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
	for (std::size_t i{ 0 }; i < l_result.size(); ++i)
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

			if (p_enemy_sets.at(l_enemy_set).size() >= c::LENGTH_M66_ENEMY_SET_DATA)
				throw std::runtime_error("Too many enemies in enemy set " + std::to_string(l_enemy_set + 1));

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
		if (l_enemy_data.size() > c::LENGTH_M66_ENEMY_DATA)
			throw std::runtime_error("Too many enemies in level " + std::to_string(lvl_i + 1));
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
		for (int y{ 0 }; y < c::LEVEL_H; ++y)
			for (int x{ 0 }; x < c::LEVEL_W; ++x) {
				byte l_value{ 0xf8 };
				auto l_wall = l_level.get_wall_type(x, y);
				if (l_wall == skc::Wall::Brown)
					l_value = 0x90;
				else if (l_wall == skc::Wall::None)
					l_value = 0x10;
				set_block(l_io_rom_data, l_offset, std::make_pair(x, y), l_value);
			}

		// make mirrors visible if they should be, by adding item 0x05 at the mirror locations
		for (std::size_t m{ 0 }; m < 2; ++m)
			if (is_mirror_visible(l_level, m))
				set_block(l_io_rom_data, l_offset, l_level.get_spawn_position(m), c::ITEM_NO_DEMON_MIRROR);

		// patch items
		const auto& l_items = l_level.get_items();
		for (const auto& l_item : l_items)
			set_block(l_io_rom_data, l_offset, l_item.get_position(), l_item.get_element_no());

		// patch metadata
		l_io_rom_data.at(l_offset + c::OFFSET_M66_LOCAL_META) = 0;
		l_io_rom_data.at(l_offset + c::OFFSET_M66_LOCAL_META + 1) = 1;
		l_io_rom_data.at(l_offset + c::OFFSET_M66_LOCAL_META + 2) = 0;
		l_io_rom_data.at(l_offset + c::OFFSET_M66_LOCAL_META + 3) = 1;
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

// utility functions

// change the ROM mapper from 03 to 66 - expand
std::vector<byte> skc::m66::change_mapper(const std::vector<byte>& p_rom3_data) {
	std::vector<byte> result(c::ROM_M66_FILE_SIZE, 0x0);

	for (std::size_t i{ 0 }; i < 32784; ++i)
		result[i] = p_rom3_data[i];
	for (std::size_t i{ 0 }; i < 16; ++i)
		result[65536 + i] = p_rom3_data[32768 + i];
	for (std::size_t i{ 0 }; i < 32768; ++i)
		result[65552 + i] = p_rom3_data[32784 + i];

	result[4] = 4;
	result[6] = 32;
	result[7] = 64;
	result[255] = 0;
	result[256] = 1;
	result[257] = 2;
	result[258] = 3;
	result[3376] = 0;
	result[6287] = 234;
	result[6288] = 234;
	result[6289] = 234;

	for (std::size_t i{ 0 }; i < c::COUNT_M66_LEVELS; ++i) {
		result[23804 + i] = 160;
		result[23857 + i] = 7;
		result[27180 + i] = 144;
		result[27233 + i] = 7;
	}

	std::vector<byte> l_a1{ 16,189,0,154,157,207,7,202,208,247,76,208,7,96,169,19,157,17,128,32,1,128,169,3,157,17,128,76,0,154,0,154 };
	for (std::size_t i{ 0 }; i < l_a1.size(); ++i)
		result[6659 + i] = l_a1[i];

	std::vector<byte> l_a2{ 64,173,40,4,24,105,191,133,1,169,255,133,0,160,192,173,40,4,201,48,240,34,165,124,106,144,29,177,0,201,248,240,23,41,63,201,46,144,17,177,0,41,128,42,144,5,169,144,24,144,7,169,16,24,144,2,177,0,153,19,3,136,208,207,160,16,169,248,153,3,3,153,211,3,136,208,247,173,40,4,24,105,192,133,1,169,191,133,0,160,64,177,0,153,143,7,136,208,248,173,40,4,10,10,10,10,24,105,0,133,0,173,40,4,74,74,74,74,24,105,245,133,1,56,165,0,233,1,133,0,165,1,233,0,133,1,160,16,177,0,153,127,7,136,208,248,96 };
	for (std::size_t i{ 0 }; i < l_a2.size(); ++i)
		result[32784 + i] = l_a2[i];

	result[23568] = 128;
	result[23569] = 136;
	result[23584] = 7;
	result[23585] = 7;
	result[23600] = 192;
	result[23601] = 200;
	result[23617] = 7;
	result[23618] = 7;

	for (std::size_t i{ 0 }; i < 48; ++i)
		result[16370 + i] = 0;

	return result;
}

std::vector<std::vector<byte>> skc::m66::expand_enemy_sets(
	const std::vector<std::vector<byte>>& p_enemy_sets,
	const std::vector<skc::Level>& p_levels
) {
	std::vector<std::vector<byte>> result;

	for (std::size_t i{ 0 }; i < p_levels.size(); ++i) {
		for (std::size_t l_nmi_set_no{ 0 }; l_nmi_set_no < 2; ++l_nmi_set_no) {
			result.push_back(p_enemy_sets.at(
				p_levels[i].get_spawn_enemies(l_nmi_set_no)
			));
		}
	}

	return result;
}

std::vector<std::vector<bool>> skc::m66::expand_drop_schedules(
	const std::vector<std::vector<bool>>& p_drop_scheds,
	const std::vector<skc::Level>& p_levels
) {
	std::vector<std::vector<bool>> result;

	for (std::size_t i{ 0 }; i < p_levels.size(); ++i) {
		for (std::size_t l_nmi_set_no{ 0 }; l_nmi_set_no < 2; ++l_nmi_set_no) {
			result.push_back(p_drop_scheds.at(
				p_levels[i].get_spawn_schedule(l_nmi_set_no)
			));
		}
	}

	return result;
}

// level utility functions
bool skc::m66::is_mirror_visible(const skc::Level& p_level, std::size_t p_mirror_no) {
	auto l_pos = p_level.get_spawn_position(p_mirror_no);
	if (skc::Level::is_position_visible(l_pos)) {
		for (const auto& l_item : p_level.get_items())
			if (l_pos == l_item.get_position() && l_item.get_element_no() != c::ITEM_NO_DEMON_MIRROR)
				return false;
		return true;
	}
	else
		return false;
}

// if a demon mirror item (0x05) is present on top of a mirror, and no other items are present there
// remove the demon mirror from the item list
// it will be added back automatically when saving the expanded ROM
void skc::m66::mirror_item_cleanup(skc::Level& p_level) {
	auto l_pos_01{ p_level.get_spawn_position(0) };
	auto l_pos_02{ p_level.get_spawn_position(1) };

	int l_ind_01{ p_level.get_item_index(l_pos_01) };
	int l_ind_02{ p_level.get_item_index(l_pos_02) };

	int l_max_ind = std::max(l_ind_01, l_ind_02);
	int l_min_ind = std::min(l_ind_01, l_ind_02);

	// delete items in the right order!
	if (l_max_ind != -1 && p_level.get_items().at(l_max_ind).get_element_no() == c::ITEM_NO_DEMON_MIRROR)
		p_level.delete_item(l_max_ind);
	if (l_min_ind != -1 && p_level.get_items().at(l_min_ind).get_element_no() == c::ITEM_NO_DEMON_MIRROR)
		p_level.delete_item(l_min_ind);
}
