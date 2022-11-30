#include "klib_util.h"

std::vector<std::string> klib::util::string_split_strings(const std::string& p_values, char p_delimeter) {
	std::vector<std::string> result;

	std::size_t l_last = 0;
	std::size_t l_next = p_values.find(p_delimeter);

	while (l_next != std::string::npos) {
		auto l_next_num = p_values.substr(l_last, l_next - l_last);
		result.push_back(l_next_num);

		l_last = l_next + 1;
		l_next = p_values.find(p_delimeter, l_last);
	}
	auto l_next_num = p_values.substr(l_last, l_next - l_last);
	if (!l_next_num.empty())
		result.push_back(l_next_num);

	return result;
}
