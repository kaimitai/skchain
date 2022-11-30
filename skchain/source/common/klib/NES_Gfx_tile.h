#ifndef KLIB_NES_GFX_TILE_H
#define KLIB_NES_GFX_TILE_H

#include <vector>
#include "NES_Palette.h"

using byte = unsigned char;

namespace klib {

	namespace c {
		constexpr int NES_TILE_W{ 8 };
	}

	class NES_Gfx_tile {

		std::vector<std::vector<byte>> m_pixels;

	public:
		NES_Gfx_tile(const std::vector<byte>& p_bytes);
		byte get_palette_index(int p_w, int p_y,
			bool p_flip_v = false,
			bool p_flip_h = false) const;
	};

}

#endif
