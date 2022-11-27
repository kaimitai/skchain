#ifndef KLIB_NES_PALETTE_H
#define KLIB_NES_PALETTE_H

#include <array>
#include <vector>

using byte = unsigned char;

namespace klib {

	struct NES_Color {
		byte m_r, m_g, m_b;

		constexpr NES_Color(byte p_r, byte p_g, byte p_b);
	};

	class NES_Palette {
		static std::vector<NES_Color> ms_nes_colors;
		std::vector<byte> m_colors;

	public:
		NES_Palette(const std::vector<unsigned char>& p_palette);
		NES_Color get_rgb_color(std::size_t p_color_no) const;
		byte get_nes_palette_index(std::size_t p_color_no) const;

		static NES_Color get_nes_color(std::size_t p_color_no);
	};

}

#endif
