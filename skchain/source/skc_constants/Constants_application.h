#ifndef SKC_CONSTANTS_APPLICATION_H
#define SKC_CONSTANTS_APPLICATION_H

#include <cstddef>

namespace skc {
	namespace c {

		constexpr char APP_VERSION[]{ "1.1-snapshot" };
		constexpr char APP_TITLE[]{ "Solomon's Keychain v1.1-snapshot" };
		constexpr int APP_W{ 1024 + 435 }, APP_H{ 768 };

		constexpr char FILENAME_CONFIG_XML[]{ "skc_config.xml" };
		constexpr char FILENAME_ERROR_LOG[]{ "skc_errors.log" };
		constexpr char FILENAME_IMGUI_INI[]{ "skc_windows.ini" };
		constexpr std::size_t LOG_MESSAGE_MAX_SIZE{ 25 };

		constexpr std::size_t ROM_FILE_SIZE{ 65552 };
		constexpr std::size_t ROM_M66_FILE_SIZE{ 98320 };

		constexpr int MSG_CODE_INFO{ 0 };
		constexpr int MSG_CODE_WARNING{ MSG_CODE_INFO + 1 };
		constexpr int MSG_CODE_SUCCESS{ MSG_CODE_WARNING + 1 };
		constexpr int MSG_CODE_ERROR{ MSG_CODE_SUCCESS + 1 };

		constexpr char TXT_UNKNOWN[]{ "Unknown" };
		constexpr char TXT_POSITION[]{ "Position" };
		constexpr char TXT_PROPERTIES[]{ "Properties" };
		constexpr char TXT_X_POS[]{ "x-pos" };
		constexpr char TXT_Y_POS[]{ "y-pos" };

		// render toggle indexes
		constexpr std::size_t TOGGLE_GRID_IDX{ 0 };
		constexpr std::size_t TOGGLE_META_IDX{ TOGGLE_GRID_IDX + 1 };
		constexpr std::size_t TOGGLE_ITEM_IDX{ TOGGLE_META_IDX + 1 };
		constexpr std::size_t TOGGLE_ENEMY_IDX{ TOGGLE_ITEM_IDX + 1 };
		constexpr char TXT_GRID[]{ "Grid" };
		constexpr char TXT_META[]{ "Meta" };
		constexpr char TXT_ITEMS[]{ "Items" };
		constexpr char TXT_ENEMIES[]{ "Enemies" };

		// initial window positions and sizes
		constexpr int WIN_MAIN_X{ 715 };
		constexpr int WIN_MAIN_Y{ 425 };
		constexpr int WIN_MAIN_W{ 420 };
		constexpr int WIN_MAIN_H{ 340 };

		constexpr int WIN_BOARD_X{ 5 };
		constexpr int WIN_BOARD_Y{ 5 };
		constexpr int WIN_BOARD_W{ 705 };
		constexpr int WIN_BOARD_H{ 705 };

		constexpr int WIN_EP_X{ 715 };
		constexpr int WIN_EP_Y{ 5 };
		constexpr int WIN_EP_W{ 420 };
		constexpr int WIN_EP_H{ 415 };

		constexpr int WIN_STW_X{ 1140 };
		constexpr int WIN_STW_Y{ 5 };
		constexpr int WIN_STW_W{ 315 };
		constexpr int WIN_STW_H{ 760 };

		constexpr int WIN_DS_X{ 10 };
		constexpr int WIN_DS_Y{ 10 };
		constexpr int WIN_DS_W{ 225 };
		constexpr int WIN_DS_H{ 295 };

		constexpr int WIN_ENEMYSET_X{ 240 };
		constexpr int WIN_ENEMYSET_Y{ 10 };
		constexpr int WIN_ENEMYSET_W{ 330 };
		constexpr int WIN_ENEMYSET_H{ 225 };

		// region codes
		constexpr char REGION_EU[]{ "EU" };
		constexpr char REGION_JP[]{ "JP" };
		constexpr char REGION_US[]{ "US" };
	}
}

#endif
