#ifndef SKC_TILE_METADATA_H
#define SKC_TILE_METADATA_H

#include <vector>

namespace skc {

	class SKC_Tile_metadata {
		friend class SKC_Tile_definition;

		std::size_t m_tile_no;
		std::size_t m_palette_no;

	public:
		SKC_Tile_metadata(std::size_t p_tile_no, std::size_t p_palette_no);
		SKC_Tile_metadata(void) = default;
	};

	class SKC_Tile_definition {
		std::vector<std::vector<SKC_Tile_metadata>> m_tile_metadata;
		bool m_transparent;

	public:
		SKC_Tile_definition(int p_w, int p_h, bool p_transparent = false);
		void add_tile_metadata(const SKC_Tile_metadata& p_metadata, std::size_t p_x, std::size_t p_y);

		int get_w(void) const;
		int get_h(void) const;
		bool is_transparent(void) const;
		std::size_t get_nes_tile_no(int p_x, int p_y) const;
		std::size_t get_palette_no(int p_x, int p_y) const;
	};
}

#endif
