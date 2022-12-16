#ifndef SKC_CONSTANTS_APPLICATION_H
#define SKC_CONSTANTS_APPLICATION_H

#include <cstddef>

namespace skc {
	namespace c {

		constexpr char APP_VERSION[]{ "0.1" };
		constexpr char APP_TITLE[]{ "Solomon's Keychain v0.1" };
		constexpr int APP_W{ 1024 + 450 }, APP_H{ 768 };

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
	}
}

#endif
