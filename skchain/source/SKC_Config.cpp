#include "SKC_Config.h"
#include "./common/klib/klib_util.h"
#include "./common/pugixml/pugixml.hpp"
#include "./common/pugixml/pugiconfig.hpp"
#include "./skc_constants/Constants_xml.h"
#include "./skc_constants/Constants_application.h"
#include "./skc_constants/Constants_level.h"
#include <stdexcept>

skc::SKC_Config::SKC_Config(const std::vector<byte>& p_rom_data) :
	m_rom_data{ p_rom_data },
	m_descriptions{ std::vector<std::vector<std::string>>(3, std::vector<std::string>(256, c::TXT_UNKNOWN)) }
{
	this->load_config_xml();
}

void skc::SKC_Config::load_config_xml(void) {
	pugi::xml_document doc;
	if (!doc.load_file(skc::c::FILENAME_CONFIG_XML))
		throw std::runtime_error("Could not load configuration xml");

	auto n_meta = doc.child(skc::c::XML_TAG_META);
	auto n_rom_meta = n_meta.child(c::XML_TAG_ROM_METADATA);

	m_offset_gfx = klib::util::string_to_numeric<std::size_t>(
		n_rom_meta.child(c::XML_TAG_OFFSET_GFX).attribute(c::XML_ATTR_OFFSET).as_string());
	m_offset_blocks = klib::util::string_to_numeric<std::size_t>(
		n_rom_meta.child(c::XML_TAG_OFFSET_BLOCKS).attribute(c::XML_ATTR_OFFSET).as_string());
	m_offset_enemy_table = klib::util::string_to_numeric<std::size_t>(
		n_rom_meta.child(c::XML_TAG_OFFSET_ENEMIES).attribute(c::XML_ATTR_OFFSET).as_string());
	m_offset_item_table = klib::util::string_to_numeric<std::size_t>(
		n_rom_meta.child(c::XML_TAG_OFFSET_ITEMS).attribute(c::XML_ATTR_OFFSET).as_string());

	m_rom_ram_diff = klib::util::string_to_numeric<unsigned int>(
		n_rom_meta.child(c::XML_TAG_ROM_RAM_DIFF).attribute(c::XML_ATTR_VALUE).as_string());
	m_level_count = klib::util::string_to_numeric<unsigned int>(
		n_rom_meta.child(c::XML_TAG_LEVEL_COUNT).attribute(c::XML_ATTR_VALUE).as_string());
	m_gfx_tile_count = klib::util::string_to_numeric<unsigned int>(
		n_rom_meta.child(c::XML_TAG_NES_TILE_COUNT).attribute(c::XML_ATTR_VALUE).as_string());

	m_level_tilesets = klib::util::string_split<std::size_t>(
		n_rom_meta.child(c::XML_TAG_LEVEL_TILESETS).attribute(c::XML_ATTR_VALUE).as_string(),
		',');

	auto n_metadatas = n_meta.child(c::XML_TAG_MD_DEFINITIONS);
	for (auto n_md = n_metadatas.child(c::XML_TAG_METADATA);
		n_md; n_md = n_md.next_sibling(c::XML_TAG_METADATA)) {

		m_descriptions[c::ELM_TYPE_METADATA].at(
			klib::util::string_to_numeric<std::size_t>(n_md.attribute(c::XML_ATTR_NO).as_string())
		) = n_md.attribute(c::XML_ATTR_DESCRIPTION).as_string();
	}

	auto n_tilepickers = n_meta.child(c::XML_TAG_TILE_PICKERS);
	for (auto n_tilepicker = n_tilepickers.child(c::XML_TAG_TILE_PICKER);
		n_tilepicker;
		n_tilepicker = n_tilepicker.next_sibling(c::XML_TAG_TILE_PICKER)) {

		std::vector<std::pair<std::string, std::vector<byte>>> l_tile_picker;

		for (auto n_section = n_tilepicker.child(c::XML_TAG_SECTION);
			n_section; n_section = n_section.next_sibling(c::XML_TAG_SECTION)) {

			std::string l_section = n_section.attribute(c::XML_ATTR_DESCRIPTION).as_string();
			auto l_tile_nos = klib::util::string_split<byte>(
				n_section.attribute(c::XML_ATTR_VALUE).as_string(),
				',');

			l_tile_picker.push_back(std::make_pair(l_section, l_tile_nos));
		}

		m_tile_pickers.push_back(l_tile_picker);
	}
}

const std::vector<byte>& skc::SKC_Config::get_rom_data(void) const {
	return m_rom_data;
}

std::size_t skc::SKC_Config::get_offset_gfx(void) const {
	return m_offset_gfx;
}

std::size_t skc::SKC_Config::get_offset_block_data(void) const {
	return m_offset_blocks;
}

std::size_t skc::SKC_Config::get_offset_enemy_table_lo(void) const {
	return m_offset_enemy_table;
}

std::size_t skc::SKC_Config::get_offset_item_table_lo(void) const {
	return m_offset_item_table;
}

std::size_t skc::SKC_Config::get_offset_enemy_table_hi(void) const {
	return m_offset_enemy_table + m_level_count;
}

std::size_t skc::SKC_Config::get_offset_item_table_hi(void) const {
	return m_offset_item_table + m_level_count;
}

std::size_t skc::SKC_Config::get_offset_enemy_data(void) const {
	return m_offset_enemy_table + 2 * m_level_count;
}

std::size_t skc::SKC_Config::get_offset_item_data(void) const {
	return m_offset_item_table + 2 * m_level_count;
}

unsigned int skc::SKC_Config::get_level_count(void) const {
	return m_level_count;
}

unsigned int skc::SKC_Config::get_nes_tile_count(void) const {
	return m_gfx_tile_count;
}

std::size_t skc::SKC_Config::get_rom_address_from_ram(std::size_t p_ram_address) const {
	return p_ram_address - m_rom_ram_diff;
}

std::size_t skc::SKC_Config::get_ram_address_from_rom(std::size_t p_rom_address) const {
	return p_rom_address + m_rom_ram_diff;
}

std::size_t skc::SKC_Config::get_level_tileset(std::size_t p_level_no) const {
	return m_level_tilesets.at(p_level_no);
}

const std::string& skc::SKC_Config::get_description(std::size_t p_element_type,
	byte p_element_no) const {
	return m_descriptions.at(p_element_type).at(p_element_no);
}

const std::vector<std::pair<std::string, std::vector<byte>>>& skc::SKC_Config::get_tile_picker(std::size_t p_element_type) const {
	return m_tile_pickers.at(p_element_type);
}
