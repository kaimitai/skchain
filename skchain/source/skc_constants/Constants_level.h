#ifndef SKC_CONSTANTS_LEVEL_H
#define SKC_CONSTANTS_LEVEL_H

using byte = unsigned char;

namespace skc {

	namespace c {

		constexpr byte DEFAULT_KEY_STATUS{ 0x01 };
		constexpr byte DEFAULT_SPAWN_RATE{ 0x02 };
		constexpr byte DEFAULT_SPAWN_VALUE{ 0x27 };

		constexpr byte ITEM_OFFSET_SPAWN01_SCHEDULE{ 0 };
		constexpr byte ITEM_OFFSET_SPAWN02_SCHEDULE{ ITEM_OFFSET_SPAWN01_SCHEDULE + 1 };
		constexpr byte ITEM_OFFSET_SPAWN01_ENEMIES{ ITEM_OFFSET_SPAWN02_SCHEDULE + 1 };
		constexpr byte ITEM_OFFSET_SPAWN02_ENEMIES{ ITEM_OFFSET_SPAWN01_ENEMIES + 1 };
		constexpr byte ITEM_OFFSET_KEY_STATUS{ ITEM_OFFSET_SPAWN02_ENEMIES + 1 };
		constexpr byte ITEM_OFFSET_DOOR_POS{ ITEM_OFFSET_KEY_STATUS + 1 };
		constexpr byte ITEM_OFFSET_KEY_POS{ ITEM_OFFSET_DOOR_POS + 1 };
		constexpr byte ITEM_OFFSET_START_POS{ ITEM_OFFSET_KEY_POS + 1 };
		constexpr byte ITEM_OFFSET_SPAWN01{ ITEM_OFFSET_START_POS + 1 };
		constexpr byte ITEM_OFFSET_SPAWN02{ ITEM_OFFSET_SPAWN01 + 1 };
		constexpr byte ITEM_OFFSET_ITEM_DATA{ ITEM_OFFSET_SPAWN02 + 1 };

		constexpr byte ITEM_CONSTELLATION_ARIES{ 0xf0 };
		constexpr byte ITEM_CONSTELLATION_GEMINI{ 0xf1 };
		constexpr byte ITEM_CONSTELLATION_VIRGO{ 0xf2 };
		constexpr byte ITEM_CONSTELLATION_AQUARIUS{ 0xf3 };
		constexpr byte ITEM_CONSTELLATION_CANCER{ 0xf4 };
		constexpr byte ITEM_CONSTELLATION_SCORPIO{ 0xf5 };
		constexpr byte ITEM_CONSTELLATION_CAPRICORN{ 0xf6 };
		constexpr byte ITEM_CONSTELLATION_PISCES{ 0xf7 };
		constexpr byte ITEM_CONSTELLATION_TAURUS{ 0xf8 };
		constexpr byte ITEM_CONSTELLATION_LEO{ 0xf9 };
		constexpr byte ITEM_CONSTELLATION_LIBRA{ 0xfa };
		constexpr byte ITEM_CONSTELLATION_SAGITTARIUS{ 0xfb };

		constexpr byte ITEM_COPY_INDICATOR_MIN{ 0xc0 };
		constexpr byte ITEM_DELIMITER_MIN{ 0xe0 };
		constexpr byte ITEM_CONSTELLATION_MIN{ ITEM_CONSTELLATION_ARIES };
		constexpr byte ITEM_CONSTELLATION_MAX{ ITEM_CONSTELLATION_SAGITTARIUS };
		constexpr byte ITEM_COMPRESS_MAX_COUNT{ ITEM_DELIMITER_MIN - ITEM_COPY_INDICATOR_MIN };

		constexpr unsigned int LEVEL_W{ 16 }, LEVEL_H{ 12 };
		constexpr unsigned int LEVEL_BLOCK_COUNT{ LEVEL_W * LEVEL_H };
		constexpr unsigned int TILE_BITMASK_BYTE_SIZE{ (LEVEL_W * LEVEL_H) / 8 };
		constexpr unsigned int TILE_WIDTH{ 16 };
		constexpr unsigned int TILE_SCALE{ 2 };
		constexpr unsigned int TILE_GFX_SIZE{ TILE_WIDTH * TILE_SCALE };
		constexpr unsigned int SIZE_LEVEL_WALL_LAYER{ (LEVEL_W * LEVEL_H) / 8 };
		constexpr unsigned int SIZE_LEVEL_WALLS{ 2 * SIZE_LEVEL_WALL_LAYER };

		// metadata keys
		constexpr byte MD_BYTE_NO_KEY{ 0x00 };
		constexpr byte MD_BYTE_NO_DOOR{ 0x01 };
		constexpr byte MD_BYTE_NO_PLAYER_START{ 0x02 };
		constexpr byte MD_BYTE_NO_SPAWN01{ 0x03 };
		constexpr byte MD_BYTE_NO_SPAWN02{ 0x04 };
		constexpr byte MD_BYTE_NO_EMPTY_TILE{ 0x05 };
		constexpr byte MD_BYTE_NO_BLOCK_BROWN{ 0x06 };
		constexpr byte MD_BYTE_NO_BLOCK_WHITE{ 0x07 };
		constexpr byte MD_BYTE_NO_BLOCK_BW{ 0x08 };
		constexpr byte MD_BYTE_NO_PLAYER_START_LEFT{ 0x09 };
		constexpr byte MD_BYTE_NO_SOLOMONS_KEY{ 0x10 };
		constexpr byte MD_BYTE_NO_CONSTELLATION{ 0x11 };
		constexpr byte MD_BYTE_NO_META_TILE_MIN{ MD_BYTE_NO_CONSTELLATION + 1 };

		// item types
		constexpr std::size_t ELM_TYPE_METADATA{ 0 };
		constexpr std::size_t ELM_TYPE_ITEM{ 1 };
		constexpr std::size_t ELM_TYPE_ENEMY{ 2 };
	}

}

#endif
