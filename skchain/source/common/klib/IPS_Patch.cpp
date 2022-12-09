#include "IPS_Patch.h"
#include "klib_util.h"
#include <stdexcept>

namespace klib {
	namespace ips {
		constexpr char PATCH_HEADER[]{ "PATCH" };
		constexpr char PATCH_EOF[]{ "EOF" };
		constexpr std::size_t PATCH_EOF_AS_OFFSET{ 0x454f46 };
	}
}

std::vector<byte> klib::ips::generate_patch(const std::vector<byte>& p_source,
	const std::vector<byte>& p_target) {
	if (p_target.size() < p_source.size())
		throw std::runtime_error("IPS patch format does not support deletion");

	std::vector<byte> result;
	for (std::size_t i{ 0 }; i < 5; ++i)
		result.push_back(PATCH_HEADER[i]);

	for (std::size_t i{ 0 }; i < p_source.size(); ) {
		std::size_t l_offset{ i };
		std::vector<byte> l_patch_bytes;
		while (i < p_source.size() && p_source[i] != p_target[i]) {
			l_patch_bytes.push_back(p_target[i++]);
		}
		if (l_patch_bytes.size() > 0)
			generate_hunk(result, l_offset, l_patch_bytes);
		else
			++i;
	}

	std::size_t l_append_byte_count{ p_target.size() - p_source.size() };
	if (l_append_byte_count != 0) {
		write_number(result, p_source.size(), 3);
		write_number(result, l_append_byte_count, 2);
		result.insert(end(result), begin(p_target) + p_source.size(), end(p_target));
	}

	for (std::size_t i{ 0 }; i < 3; ++i)
		result.push_back(PATCH_EOF[i]);

	return result;
}

void klib::ips::copy_bytes(std::vector<byte>& pr_result, std::size_t p_patch_offset,
	const std::vector<byte>& p_source_bytes, std::size_t p_source_offset, std::size_t p_length) {

	while (pr_result.size() < p_patch_offset + p_length)
		pr_result.push_back(0x00);

	for (std::size_t i{ 0 }; i < p_length; ++i)
		pr_result.at(p_patch_offset + i) = p_source_bytes.at(p_source_offset + i);
}

void klib::ips::generate_hunk(std::vector<byte>& pr_result, std::size_t p_offset, const std::vector<byte>& p_patch_bytes) {
	bool l_all_equal{ true };
	byte l_cmp_byte{ p_patch_bytes.at(0) };
	for (std::size_t i{ 1 }; i < p_patch_bytes.size(); ++i)
		if (p_patch_bytes[i] != l_cmp_byte) {
			l_all_equal = false;
			break;
		}

	if (p_patch_bytes.size() > 3 && l_all_equal) {
		write_number(pr_result, p_offset, 3);
		write_number(pr_result, 0, 2);
		write_number(pr_result, p_patch_bytes.size(), 2);
		pr_result.push_back(l_cmp_byte);
	}
	else {
		write_number(pr_result, p_offset, 3);
		write_number(pr_result, p_patch_bytes.size(), 2);
		pr_result.insert(end(pr_result), begin(p_patch_bytes), end(p_patch_bytes));
	}
}

void klib::ips::copy_run_length(std::vector<byte>& pr_result,
	std::size_t p_offset, std::size_t p_length, byte p_byte) {

	while (pr_result.size() < p_offset + p_length)
		pr_result.push_back(0x00);

	for (std::size_t i{ 0 }; i < p_length; ++i)
		pr_result.at(p_offset + i) = p_byte;
}

std::vector<byte> klib::ips::apply_patch(const std::vector<byte>& p_source,
	const std::vector<byte>& p_patch_bytes) {
	std::vector<byte> result{ p_source };
	std::size_t l_source_index{ 0 };

	for (; l_source_index < 5; ++l_source_index)
		if (p_patch_bytes.at(l_source_index) != PATCH_HEADER[l_source_index])
			throw std::runtime_error("Invalid IPS patch header");

	while (true) {
		std::size_t l_target_offset{ read_number(p_patch_bytes, l_source_index, 3) };
		l_source_index += 3;

		if (l_target_offset == PATCH_EOF_AS_OFFSET)
			break;
		else {
			std::size_t l_length{ read_number(p_patch_bytes, l_source_index, 2) };
			l_source_index += 2;

			// Run-Length encoding
			if (l_length == 0) {
				std::size_t l_run_length{ read_number(p_patch_bytes, l_source_index, 2) };
				byte l_copy_byte{ p_patch_bytes.at(l_source_index + 2) };
				copy_run_length(result, l_target_offset, l_run_length, l_copy_byte);
				l_source_index += 3;
			}
			// Not RLE - push bytes
			else {
				copy_bytes(result, l_target_offset, p_patch_bytes, l_source_index, l_length);
				l_source_index += l_length;
			}
		}
	}

	return result;
}

std::size_t klib::ips::read_number(const std::vector<byte>& p_data,
	std::size_t p_offset, std::size_t p_byte_count) {
	std::size_t result{ 0 };

	for (std::size_t i{ 0 }; i < p_byte_count; ++i) {
		result *= static_cast<std::size_t>(256);
		result += p_data.at(p_offset + i);
	}

	return result;
}

void klib::ips::write_number(std::vector<byte>& pr_result, std::size_t p_value, std::size_t p_byte_count) {
	std::vector<byte> l_reverse;

	for (std::size_t i{ 0 }; i < p_byte_count; ++i) {
		l_reverse.push_back(p_value % 256);
		p_value /= 256;
	}

	for (auto iter = rbegin(l_reverse); iter != rend(l_reverse); ++iter)
		pr_result.push_back(*iter);
}
