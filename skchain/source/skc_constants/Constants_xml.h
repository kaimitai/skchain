#ifndef SKC_CONSTANTS_XML_H
#define SKC_CONSTANTS_XML_H

namespace skc {
	namespace c {

		constexpr char XML_TAG_META[]{ "skchain" };

		constexpr char XML_TAG_GFX_METADATA[]{ "gfx_metadata" };
		constexpr char XML_TAG_PALETTES[]{ "palettes" };
		constexpr char XML_TAG_PALETTE[]{ "palette" };
		constexpr char XML_TAG_TILE_DEFINITIONS[]{ "tile_definitions" };
		constexpr char XML_TAG_TILE[]{ "tile" };

		constexpr char XML_ATTR_NO[]{ "no" };
		constexpr char XML_ATTR_W[]{ "w" };
		constexpr char XML_ATTR_COLORS[]{ "colors" };
		constexpr char XML_ATTR_PALETTE_NO[]{ "palette_no" };
		constexpr char XML_ATTR_NES_TILES[]{ "nes_tiles" };

	}
}

#endif
