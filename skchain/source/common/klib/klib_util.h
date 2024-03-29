#ifndef KLIB_UTIL_H
#define KLIB_UTIL_H

#include <set>
#include <string>
#include <vector>
#include <stdexcept>

using byte = unsigned char;

namespace klib {

	namespace util {

		bool get_bit(byte p_byte, unsigned int p_position);
		void set_bit(byte& p_byte, unsigned int p_position, bool p_value);
		std::vector<std::vector<bool>> bytes_to_bitmask(const std::vector<byte>& p_bytes, int p_w, int p_h, std::size_t p_start_pos = 0);
		std::vector<byte> bitmask_to_bytes(const std::vector<std::vector<bool>>& p_bitmask);

		std::vector<bool> bytes_to_bitmask_1d(const std::vector<byte>& p_bytes, int p_w = 0, std::size_t p_start_pos = 0);
		std::vector<byte> bitmask_to_bytes(const std::vector<bool>& p_bitmask);

		std::string to_lowercase(const std::string& p_string);
		bool has_substring_caseless(const std::string& p_needle, const std::string& p_haystack);

		template<class T>
		std::vector<std::vector<bool>> vec2_to_bitmask(const std::vector<std::vector<T>>& p_data, const std::set<T>& p_true_vals) {
			std::vector<std::vector<bool>> result;

			for (const auto& l_datarow : p_data) {
				std::vector<bool> l_row;
				for (const T& l_item : l_datarow)
					l_row.push_back(p_true_vals.find(l_item) != end(p_true_vals));
				result.push_back(l_row);
			}

			return result;
		}

		template <class T>
		T string_to_numeric(const std::string& p_value) {
			if (p_value.size() >= 2 && p_value.substr(0, 2) == "0x") {

				T result{ 0 };

				for (std::size_t i{ 2 }; i < p_value.size(); ++i) {
					result *= 16;
					char cchar{ p_value[i] };
					if (cchar >= '0' && cchar <= '9')
						result += (cchar - '0');
					else if (cchar >= 'a' && cchar <= 'f')
						result += (cchar - 'a') + 10;
					else if (cchar >= 'A' && cchar <= 'F')
						result += (cchar - 'A') + 10;
					else
						throw std::runtime_error("Invalid hex-string");
				}

				return result;
			}
			else
				return atoi(p_value.c_str());
		}

		std::vector<std::string> string_split_strings(const std::string& p_values, char p_delimeter);

		template <class T>
		std::vector<T> string_split(const std::string& p_values, char p_delimeter) {
			std::vector<T> result;

			std::size_t l_last = 0;
			std::size_t l_next = p_values.find(p_delimeter);

			while (l_next != std::string::npos) {
				auto l_next_num = p_values.substr(l_last, l_next - l_last);
				result.push_back(string_to_numeric<T>(l_next_num));

				l_last = l_next + 1;
				l_next = p_values.find(p_delimeter, l_last);
			}
			auto l_next_num = p_values.substr(l_last, l_next - l_last);
			if (!l_next_num.empty())
				result.push_back(string_to_numeric<T>(l_next_num));

			return result;
		}

		template <class T>
		std::string string_join(const std::vector<T>& p_values, char p_delimiter) {
			std::string result;

			for (std::size_t i{ 0 }; i < p_values.size(); ++i)
				result += std::to_string(p_values[i]) + p_delimiter;

			if (!result.empty())
				result.pop_back();

			return result;
		}

		template <class T>
		void rot_sq_matrix_ccw(std::vector<std::vector<T>>& p_input) {

			auto cyclic_roll = [](T& a, T& b, T& c, T& d) {
				T temp = a;
				a = b;
				b = c;
				c = d;
				d = temp;
			};

			int n{ static_cast<int>(p_input.size()) };

			for (int i{ 0 }; i < n / 2; i++)
				for (int j{ 0 }; j < (n + 1) / 2; j++)
					cyclic_roll(p_input[i][j], p_input[n - 1 - j][i], p_input[n - 1 - i][n - 1 - j], p_input[j][n - 1 - i]);
		}

		template <class T>
		T validate(T p_input, T p_min_val, T p_max_val) {
			if (p_input < p_min_val)
				return p_min_val;
			else if (p_input > p_max_val)
				return p_max_val;
			else
				return p_input;
		}

		template<class T>
		std::string stringnum(T p_num, std::size_t p_length = 3) {
			std::string result{ std::to_string(p_num) };

			while (result.size() < p_length)
				result.insert(begin(result), '0');

			return result;
		}

		template<class T>
		void append_vector(std::vector<T>& p_target, const std::vector<T>& p_source) {
			p_target.insert(end(p_target), begin(p_source), end(p_source));
		}

		template<class T>
		void append_or_overwrite_vector(std::vector<T>& p_target, const std::vector<T>& p_source, std::size_t p_target_offset) {
			while (p_target.size() < p_target_offset)
				p_target.push_back(T());
			for (std::size_t i{ 0 }; i < p_source.size(); ++i) {
				if (p_target.size() < p_target_offset + i)
					p_target.push_back(p_source[i]);
				else
					p_target[p_target_offset + i] = p_source[i];
			}
		}

	}

}

#endif
