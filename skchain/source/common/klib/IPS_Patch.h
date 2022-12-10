#ifndef KLIB_IPS_PATCH_H
#define KLIB_IPS_PATCH_H

#include <vector>

using byte = unsigned char;

namespace klib {

	namespace ips {

		void copy_run_length(std::vector<byte>& p_result,
			std::size_t p_offset, std::size_t p_length, byte p_byte);
		void copy_bytes(std::vector<byte>& p_result, std::size_t p_patch_offset,
			const std::vector<byte>& p_source_bytes, std::size_t p_source_offset, std::size_t p_length);
		void generate_hunk(std::vector<byte>& pr_result, std::size_t p_offset, const std::vector<byte>& p_patch_bytes);
		std::size_t get_run_length(const std::vector<byte>& p_patch_bytes, const std::size_t p_offset);

		std::vector<byte> generate_patch(const std::vector<byte>& p_source,
			const std::vector<byte>& p_target);
		std::vector<byte> apply_patch(const std::vector<byte>& p_source,
			const std::vector<byte>& p_patch_bytes);

		std::size_t read_number(const std::vector<byte>& p_data,
			std::size_t p_offset, std::size_t p_byte_count);
		void write_number(std::vector<byte>& p_result, std::size_t p_value, std::size_t p_byte_count);

	};

}

#endif
