#ifndef SKC_ROM_EXPANDER_H
#define SKC_ROM_EXPANDER_H

#include <cstddef>
#include <vector>
#include "./../SKC_Level.h"

using byte = unsigned char;

namespace skc {

	namespace c {
		constexpr std::size_t COUNT_M66_LEVELS{ 53 };
		constexpr std::size_t LENGTH_M66_LVL_DATA{ 256 };
		constexpr std::size_t LENGTH_M66_LVL_W{ 16 };
		constexpr std::size_t LENGTH_M66_LVL_H{ 12 };

		constexpr std::size_t OFFSET_M66_LVL_DATA{ 49168 };
		constexpr std::size_t OFFSET_M66_DROP_SCHED_DATA{ OFFSET_M66_LVL_DATA + COUNT_M66_LEVELS * LENGTH_M66_LVL_DATA };

		constexpr std::size_t LENGTH_M66_MAP_DATA{ LENGTH_M66_LVL_W * LENGTH_M66_LVL_H };

		constexpr std::size_t OFFSET_M66_LOCAL_META{ LENGTH_M66_MAP_DATA };
		constexpr std::size_t OFFSET_M66_KEY_STATUS{ OFFSET_M66_LOCAL_META + 4 };
		constexpr std::size_t OFFSET_M66_DOOR_POS{ OFFSET_M66_KEY_STATUS + 1 };
		constexpr std::size_t OFFSET_M66_KEY_POS{ OFFSET_M66_DOOR_POS + 1 };
		constexpr std::size_t OFFSET_M66_PLAYER_START_POS{ OFFSET_M66_KEY_POS + 1 };
		constexpr std::size_t OFFSET_M66_SPAWN01_POS{ OFFSET_M66_PLAYER_START_POS + 1 };
		constexpr std::size_t OFFSET_M66_SPAWN02_POS{ OFFSET_M66_SPAWN01_POS + 1 };
		constexpr std::size_t OFFSET_M66_ITEM_DELIMITER{ OFFSET_M66_SPAWN02_POS + 1 };
		constexpr std::size_t OFFSET_M66_CONSTELLATION_POS{ OFFSET_M66_ITEM_DELIMITER + 1 };

		constexpr std::size_t OFFSET_M66_LOCAL_ENEMY_DATA{ 208 };
		constexpr std::size_t LENGTH_M66_ENEMY_DATA{ 1 + 13 * 2 + 2 };

		constexpr std::size_t OFFSET_M66_LOCAL_SCHED_ENEMY_1_DATA{ 240 };
		constexpr std::size_t OFFSET_M66_LOCAL_SCHED_ENEMY_2_DATA{ OFFSET_M66_LOCAL_SCHED_ENEMY_1_DATA + 8 };
	}

	// ROM mapper 66 functions and procedures
	namespace m66 {

		// parsing
		skc::Level parse_level(const std::vector<byte>& p_rom_data,
			std::size_t p_level_no);
		std::vector<std::vector<byte>> parse_mirror_enemy_sets(const std::vector<byte>& p_rom_data);
		std::vector<std::vector<bool>> parse_mirror_drop_schedules(const std::vector<byte>& p_rom_data);

		// encoding
		void patch_mirror_drop_schedule_bytes(
			std::vector<byte>& l_io_rom_data,
			const std::vector<std::vector<bool>>& p_drop_schedules,
			const std::vector<skc::Level>& p_levels
		);
		void patch_enemy_data_bytes(
			std::vector<byte>& l_io_rom_data,
			const std::vector<skc::Level>& p_levels
		);
		void patch_mirror_enemy_set_bytes(
			std::vector<byte>& l_io_rom_data,
			const std::vector<std::vector<byte>>& p_enemy_sets,
			const std::vector<skc::Level>& p_levels
		);
		void patch_item_data_bytes(
			std::vector<byte>& l_io_rom_data,
			const std::vector<skc::Level>& p_levels
		);

		// utility functions
		std::vector<byte> change_mapper(const std::vector<byte>& p_rom3_data);
		std::vector<std::vector<byte>> expand_enemy_sets(
			const std::vector<std::vector<byte>>& p_enemy_sets,
			const std::vector<skc::Level>& p_levels
		);
		std::vector<std::vector<bool>> expand_drop_schedules(
			const std::vector<std::vector<bool>>& p_drop_scheds,
			const std::vector<skc::Level>& p_levels
		);
	}

}

#endif
