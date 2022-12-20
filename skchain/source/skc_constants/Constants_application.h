#ifndef SKC_CONSTANTS_APPLICATION_H
#define SKC_CONSTANTS_APPLICATION_H

#include <cstddef>

namespace skc {
	namespace c {

		constexpr char APP_VERSION[]{ "0.1" };
		constexpr char APP_TITLE[]{ "Solomon's Keychain v0.1" };
		constexpr int APP_W{ 1024 + 435 }, APP_H{ 768 };

		constexpr char FILENAME_CONFIG_XML[]{ "skc_config.xml" };
		constexpr char FILENAME_ERROR_LOG[]{ "skc_errors.log" };
		constexpr char FILENAME_IMGUI_INI[]{ "skc_windows.ini" };
		constexpr std::size_t LOG_MESSAGE_MAX_SIZE{ 25 };

		constexpr int MSG_CODE_INFO{ 0 };
		constexpr int MSG_CODE_WARNING{ MSG_CODE_INFO + 1 };
		constexpr int MSG_CODE_SUCCESS{ MSG_CODE_WARNING + 1 };
		constexpr int MSG_CODE_ERROR{ MSG_CODE_SUCCESS + 1 };

		constexpr char TXT_UNKNOWN[]{ "Unknown" };
		constexpr char TXT_X_POS[]{ "x-pos" };
		constexpr char TXT_Y_POS[]{ "y-pos" };

		// initial window positions and sizes
		constexpr int WIN_MAIN_X{ 1030 };
		constexpr int WIN_MAIN_Y{ 425 };
		constexpr int WIN_MAIN_W{ 420 };
		constexpr int WIN_MAIN_H{ 335 };

		constexpr int WIN_LEVEL_X{ 1030 };
		constexpr int WIN_LEVEL_Y{ 5 };
		constexpr int WIN_LEVEL_W{ 420 };
		constexpr int WIN_LEVEL_H{ 415 };

		constexpr int WIN_STW_X{ 745 };
		constexpr int WIN_STW_Y{ 560 };
		constexpr int WIN_STW_W{ 250 };
		constexpr int WIN_STW_H{ 195 };

		constexpr int WIN_DS_X{ 10 };
		constexpr int WIN_DS_Y{ 10 };
		constexpr int WIN_DS_W{ 225 };
		constexpr int WIN_DS_H{ 295 };

		constexpr int WIN_ENEMYSET_X{ 240 };
		constexpr int WIN_ENEMYSET_Y{ 10 };
		constexpr int WIN_ENEMYSET_W{ 330 };
		constexpr int WIN_ENEMYSET_H{ 225 };
		
	}
}

#endif
