#include "NES_Gfx_tile.h"
#include "klib_file.h"

klib::NES_Gfx_tile::NES_Gfx_tile(const std::vector<byte>& p_bytes) {
	for (int i{ 0 }; i < 8; ++i) {
		std::vector<byte> l_row;

		byte l_a = p_bytes.at(i);
		byte l_b = p_bytes.at(i + 8);

		for (int e{ 7 }; e >= 0; --e) {
			byte l_pal_index = klib::file::get_bit(l_a, e) +
				2 * klib::file::get_bit(l_b, e);
			l_row.push_back(l_pal_index);
		}

		m_pixels.push_back(l_row);
	}
}

byte klib::NES_Gfx_tile::get_palette_index(int p_x, int p_y,
	bool p_flip_v,
	bool p_flip_h) const {
	return m_pixels.at(
		p_flip_h ? c::NES_TILE_W - 1 - p_y : p_y).at(
			p_flip_v ? c::NES_TILE_W - 1 - p_x : p_x);
}
