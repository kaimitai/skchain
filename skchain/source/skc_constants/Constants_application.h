#ifndef SKC_CONSTANTS_APPLICATION_H
#define SKC_CONSTANTS_APPLICATION_H

namespace skc {
	namespace c {

		constexpr char APP_VERSION[]{ "0.1" };
		constexpr char APP_TITLE[]{ "Solomon's Keychain v0.1" };
		constexpr int APP_W{ 1024 + 300 }, APP_H{ 768 };

		constexpr char FILENAME_CONFIG_XML[]{ "skc_config.xml" };
		constexpr char FILENAME_ERROR_LOG[]{"skc_errors.log"};
		constexpr char FILENAME_IMGUI_INI[]{ "skc_windows.ini" };

		constexpr char TXT_UNKNOWN[]{ "Unknown" };
	}
}

#endif
