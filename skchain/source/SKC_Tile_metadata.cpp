#include "SKC_Tile_metadata.h"

skc::SKC_Tile_metadata::SKC_Tile_metadata(std::size_t p_tile_no, std::size_t p_palette_no) :
	m_tile_no{ p_tile_no },
	m_palette_no{ p_palette_no }
{}

skc::SKC_Tile_definition::SKC_Tile_definition(int p_w, int p_h, bool p_transparent) :
	m_transparent{ p_transparent }
{
	m_tile_metadata = std::vector<std::vector<SKC_Tile_metadata>>(p_h,
		std::vector<SKC_Tile_metadata>(p_w, SKC_Tile_metadata()));
}

void skc::SKC_Tile_definition::add_tile_metadata(const skc::SKC_Tile_metadata& p_metadata, std::size_t p_x, std::size_t p_y) {
	m_tile_metadata.at(p_y).at(p_x) = p_metadata;
}

int skc::SKC_Tile_definition::get_w(void) const {
	return static_cast<int>(m_tile_metadata.empty() ? 0 :
		m_tile_metadata[0].size());
}

int skc::SKC_Tile_definition::get_h(void) const {
	return static_cast<int>(m_tile_metadata.size());
}

bool skc::SKC_Tile_definition::is_transparent(void) const {
	return m_transparent;
}

std::size_t skc::SKC_Tile_definition::get_nes_tile_no(int p_x, int p_y) const {
	return m_tile_metadata.at(p_y).at(p_x).m_tile_no;
}

std::size_t skc::SKC_Tile_definition::get_palette_no(int p_x, int p_y) const {
	return m_tile_metadata.at(p_y).at(p_x).m_palette_no;
}
