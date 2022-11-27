#ifndef KLIB_NES_GFX_TILE_H
#define KLIB_NES_GFX_TILE_H

#include <vector>
#include "NES_Palette.h"

using byte = unsigned char;

namespace klib {

	class NES_Gfx_tile {

		std::vector<std::vector<byte>> m_pixels;

	public:
		NES_Gfx_tile(const std::vector<byte>& p_bytes);
	};

}

#endif
