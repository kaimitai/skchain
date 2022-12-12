#include "klib_util.h"

using byte = unsigned char;

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

bool klib::util::get_bit(byte p_byte, unsigned int p_position) {
	return (p_byte >> p_position) & 1;
}

void klib::util::set_bit(byte& p_byte, unsigned int p_position, bool p_value) {
	if (p_value)
		p_byte |= 0x01 << p_position;
	else
		p_byte &= ~(0x01 << p_position);
}

// assume that the input vector has a width which is 0 mod 8
std::vector<byte> klib::util::bitmask_to_bytes(const std::vector<std::vector<bool>>& p_bitmask) {
	std::vector<byte> result;

	for (const auto& l_row : p_bitmask)
		for (std::size_t i{ 0 }; i < l_row.size(); i += 8) {
			byte l_byte{ 0 };
			for (std::size_t b{ 0 }; b < 8; ++b) {
				l_byte *= 2;
				l_byte += l_row[i + b];
			}
			result.push_back(l_byte);
		}

	return result;
}

std::vector<byte> klib::util::bitmask_to_bytes(const std::vector<bool>& p_bitmask) {
	std::vector<std::vector<bool>> l_tmp_2d;
	l_tmp_2d.push_back(p_bitmask);

	return bitmask_to_bytes(l_tmp_2d);
}

std::vector<std::vector<bool>> klib::util::bytes_to_bitmask(const std::vector<byte>& p_bytes, int p_w, int p_h, std::size_t p_start_pos) {
	std::vector<std::vector<bool>> result;

	for (int j{ 0 }; j < p_h; ++j) {
		std::vector<bool> l_row;
		for (int i{ 0 }; i < p_w / 8; ++i) {
			byte l_b{ p_bytes.at(p_start_pos + static_cast<std::size_t>(j * p_w / 8 + i)) };
			for (int i{ 7 }; i >= 0; --i)
				l_row.push_back(klib::util::get_bit(l_b, i));
		}
		result.push_back(l_row);
	}

	return result;
}

std::vector<bool> klib::util::bytes_to_bitmask_1d(const std::vector<byte>& p_bytes, int p_w, std::size_t p_start_pos) {
	auto l_tmp_2d{ bytes_to_bitmask(p_bytes,
		p_w == 0 ? static_cast<int>(8 * (p_bytes.size() - p_start_pos)) : p_w,
		1, p_start_pos) };
	return l_tmp_2d.at(0);
}
