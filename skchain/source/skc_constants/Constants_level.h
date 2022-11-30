#ifndef SKC_CONSTANTS_LEVEL_H
#define SKC_CONSTANTS_LEVEL_H

using byte = unsigned char;

namespace skc {

	namespace c {

		constexpr byte DEFAULT_KEY_STATUS{ 0x01 };
		constexpr byte DEFAULT_SPAWN_RATE{ 0x02 };
		constexpr byte DEFAULT_SPAWN_VALUE{ 0x27 };

		constexpr byte ITEM_OFFSET_KEY_STATUS{ 4 };
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

		constexpr unsigned int LEVEL_W{ 16 }, LEVEL_H{ 12 };
		constexpr unsigned int SIZE_LEVEL_WALL_LAYER{ (LEVEL_W * LEVEL_H) / 8 };
		constexpr unsigned int SIZE_LEVEL_WALLS{ 2 * SIZE_LEVEL_WALL_LAYER };
	}

}

#endif
