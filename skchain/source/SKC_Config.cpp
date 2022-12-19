#include "SKC_Config.h"
#include "./common/klib/klib_file.h"
#include "./common/klib/klib_util.h"
#include "./common/pugixml/pugixml.hpp"
#include "./common/pugixml/pugiconfig.hpp"
#include "./skc_constants/Constants_xml.h"
#include "./skc_constants/Constants_application.h"
#include "./skc_constants/Constants_level.h"
#include <filesystem>
#include <stdexcept>

skc::SKC_Config::SKC_Config(const std::string& p_base_dir, const std::string& p_filename) :
	m_descriptions{ std::vector<std::vector<std::string>>(3, std::vector<std::string>(256, c::TXT_UNKNOWN)) },
	m_base_dir{ p_base_dir }
{
	this->load_config_xml(get_config_xml_full_path());
	m_rom_data = klib::file::read_file_as_bytes(p_filename);
	add_message("Loaded " + p_filename);
}

void skc::SKC_Config::load_config_xml(const std::string& p_config_file_path) {
	pugi::xml_document doc;
	if (!doc.load_file(p_config_file_path.c_str()))
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
	m_offset_mirror_rate_table = klib::util::string_to_numeric<std::size_t>(
		n_rom_meta.child(c::XML_TAG_OFFSET_MIRROR_RATE_TABLE).attribute(c::XML_ATTR_OFFSET).as_string());
	m_offset_mirror_enemy_table = klib::util::string_to_numeric<std::size_t>(
		n_rom_meta.child(c::XML_TAG_OFFSET_MIRROR_ENEMY_TABLE).attribute(c::XML_ATTR_OFFSET).as_string());

	m_rom_ram_diff = klib::util::string_to_numeric<unsigned int>(
		n_rom_meta.child(c::XML_TAG_ROM_RAM_DIFF).attribute(c::XML_ATTR_VALUE).as_string());
	m_level_count = klib::util::string_to_numeric<unsigned int>(
		n_rom_meta.child(c::XML_TAG_LEVEL_COUNT).attribute(c::XML_ATTR_VALUE).as_string());
	m_mirror_rate_count = klib::util::string_to_numeric<unsigned int>(
		n_rom_meta.child(c::XML_TAG_MIRROR_RATE_COUNT).attribute(c::XML_ATTR_VALUE).as_string());
	m_mirror_enemy_count = klib::util::string_to_numeric<unsigned int>(
		n_rom_meta.child(c::XML_TAG_MIRROR_ENEMY_COUNT).attribute(c::XML_ATTR_VALUE).as_string());

	m_length_mr_data = klib::util::string_to_numeric<unsigned int>(
		n_rom_meta.child(c::XML_TAG_LENGTH_MIRROR_RATE_DATA).attribute(c::XML_ATTR_VALUE).as_string());
	m_length_me_data = klib::util::string_to_numeric<unsigned int>(
		n_rom_meta.child(c::XML_TAG_LENGTH_MIRROR_ENEMY_DATA).attribute(c::XML_ATTR_VALUE).as_string());
	m_length_item_data = klib::util::string_to_numeric<unsigned int>(
		n_rom_meta.child(c::XML_TAG_LENGTH_ITEM_DATA).attribute(c::XML_ATTR_VALUE).as_string());
	m_length_enemy_data = klib::util::string_to_numeric<unsigned int>(
		n_rom_meta.child(c::XML_TAG_LENGTH_ENEMY_DATA).attribute(c::XML_ATTR_VALUE).as_string());

	m_gfx_tile_count = klib::util::string_to_numeric<unsigned int>(
		n_rom_meta.child(c::XML_TAG_NES_TILE_COUNT).attribute(c::XML_ATTR_VALUE).as_string());

	m_level_palettes = klib::util::string_split<std::size_t>(
		n_rom_meta.child(c::XML_TAG_LEVEL_PALETTES).attribute(c::XML_ATTR_VALUE).as_string(),
		',');

	auto n_item_bitmasks = n_rom_meta.child(c::XML_TAG_ITEM_BITMASKS);
	for (auto n_item_bitmask = n_item_bitmasks.child(c::XML_TAG_ITEM_BITMASK); n_item_bitmask;
		n_item_bitmask = n_item_bitmask.next_sibling(c::XML_TAG_ITEM_BITMASK)) {
		std::size_t l_level_no{ klib::util::string_to_numeric<std::size_t>(n_item_bitmask.attribute(c::XML_ATTR_LEVEL_NO).as_string()) };
		byte l_item_no{ klib::util::string_to_numeric<byte>(n_item_bitmask.attribute(c::XML_ATTR_ITEM_NO).as_string()) };
		std::size_t l_offset{ klib::util::string_to_numeric<std::size_t>(n_item_bitmask.attribute(c::XML_ATTR_OFFSET).as_string()) };
		m_item_bitmasks.insert(std::make_pair(l_level_no, std::make_pair(l_item_no, l_offset)));
	}

	auto n_lvl_meta_items = n_rom_meta.child(c::XML_TAG_LEVEL_META_ITEMS);
	for (auto n_lvl_meta = n_lvl_meta_items.child(c::XML_TAG_LEVEL_META_ITEM); n_lvl_meta;
		n_lvl_meta = n_lvl_meta.next_sibling(c::XML_TAG_LEVEL_META_ITEM)) {
		std::size_t l_level_no{ klib::util::string_to_numeric<std::size_t>(n_lvl_meta.attribute(c::XML_ATTR_LEVEL_NO).as_string()) };
		std::size_t l_animation{ klib::util::string_to_numeric<std::size_t>(n_lvl_meta.attribute(c::XML_ATTR_ANIMATION).as_string()) };
		std::size_t l_offset{ klib::util::string_to_numeric<std::size_t>(n_lvl_meta.attribute(c::XML_ATTR_OFFSET).as_string()) };
		bool l_transparent{ n_lvl_meta.attribute(c::XML_ATTR_TRANSPARENT).as_bool() };
		auto l_pos_vec{ klib::util::string_split<int>(n_lvl_meta.attribute(c::XML_ATTR_POSITION).as_string(), ',') };
		std::pair<int, int> l_position{ l_offset == 0 ? std::make_pair(l_pos_vec.at(0), l_pos_vec.at(1)) : std::make_pair(0,0) };
		std::string l_description{ n_lvl_meta.attribute(c::XML_ATTR_DESCRIPTION).as_string() };

		m_meta_items.push_back(Metadata_item(l_level_no, l_animation, l_description, l_transparent, l_offset, l_position));
	}

	auto n_metadatas = n_meta.child(c::XML_TAG_MD_DEFINITIONS);
	for (auto n_md = n_metadatas.child(c::XML_TAG_METADATA);
		n_md; n_md = n_md.next_sibling(c::XML_TAG_METADATA)) {

		m_descriptions[c::ELM_TYPE_METADATA].at(
			klib::util::string_to_numeric<std::size_t>(n_md.attribute(c::XML_ATTR_NO).as_string())
		) = n_md.attribute(c::XML_ATTR_DESCRIPTION).as_string();
	}

	auto n_items = n_meta.child(c::XML_TAG_ITEM_DEFINITIONS);
	for (auto n_item = n_items.child(c::XML_TAG_ITEM);
		n_item; n_item = n_item.next_sibling(c::XML_TAG_ITEM)) {

		m_descriptions[c::ELM_TYPE_ITEM].at(
			klib::util::string_to_numeric<std::size_t>(n_item.attribute(c::XML_ATTR_NO).as_string())
		) = n_item.attribute(c::XML_ATTR_DESCRIPTION).as_string();
	}

	auto n_enemies = n_meta.child(c::XML_TAG_ENEMY_DEFINITIONS);
	for (auto n_enemy = n_enemies.child(c::XML_TAG_ENEMY);
		n_enemy; n_enemy = n_enemy.next_sibling(c::XML_TAG_ENEMY)) {

		m_descriptions[c::ELM_TYPE_ENEMY].at(
			klib::util::string_to_numeric<std::size_t>(n_enemy.attribute(c::XML_ATTR_NO).as_string())
		) = n_enemy.attribute(c::XML_ATTR_DESCRIPTION).as_string();
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

std::size_t skc::SKC_Config::get_offset_block_data(std::size_t p_level_no) const {
	return m_offset_blocks + p_level_no * c::SIZE_LEVEL_WALLS;
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
	return m_offset_enemy_table + static_cast<std::size_t>(2) * m_level_count;
}

std::size_t skc::SKC_Config::get_offset_item_data(void) const {
	return m_offset_item_table + static_cast<std::size_t>(2) * m_level_count;
}

std::size_t skc::SKC_Config::get_offset_generic_data(std::size_t p_table_offset, std::size_t p_table_entry_count,
	std::size_t p_data_index) const {

	std::size_t l_offset_hi{ m_rom_data.at(p_table_offset + p_table_entry_count + p_data_index) };
	std::size_t l_offset_lo{ m_rom_data.at(p_table_offset + p_data_index) };
	std::size_t l_ram_offset{ 256 * l_offset_hi + l_offset_lo };

	return get_rom_address_from_ram(l_ram_offset);
}

std::size_t skc::SKC_Config::get_offset_enemy_data(std::size_t p_level_no) const {
	return get_offset_generic_data(m_offset_enemy_table, m_level_count, p_level_no);
}

std::size_t skc::SKC_Config::get_offset_item_data(std::size_t p_level_no) const {
	return get_offset_generic_data(m_offset_item_table, m_level_count, p_level_no);
}

std::size_t skc::SKC_Config::get_offset_mirror_rate_data(std::size_t p_index) const {
	return get_offset_generic_data(m_offset_mirror_rate_table, m_mirror_rate_count, p_index);
}

std::size_t skc::SKC_Config::get_offset_mirror_enemy_data(std::size_t p_index) const {
	return get_offset_generic_data(m_offset_mirror_enemy_table, m_mirror_enemy_count, p_index);
}

std::size_t skc::SKC_Config::get_offset_mirror_enemy_table_lo(void) const {
	return m_offset_mirror_enemy_table;
}

std::size_t skc::SKC_Config::get_offset_mirror_enemy_table_hi(void) const {
	return m_offset_mirror_enemy_table + m_mirror_enemy_count;
}

unsigned int skc::SKC_Config::get_level_count(void) const {
	return m_level_count;
}

unsigned int skc::SKC_Config::get_nes_tile_count(void) const {
	return m_gfx_tile_count;
}

unsigned int skc::SKC_Config::get_mirror_rate_count(void) const {
	return m_mirror_rate_count;
}

unsigned int skc::SKC_Config::get_mirror_enemy_count(void) const {
	return m_mirror_enemy_count;
}

std::size_t skc::SKC_Config::get_length_mirror_rate_data(void) const {
	return m_length_mr_data;
}

std::size_t skc::SKC_Config::get_length_mirror_enemy_data(void) const {
	return m_length_me_data;
}

std::size_t skc::SKC_Config::get_length_item_data(void) const {
	return m_length_item_data;
}

std::size_t skc::SKC_Config::get_length_enemy_data(void) const {
	return m_length_enemy_data;
}

std::size_t skc::SKC_Config::get_rom_address_from_ram(std::size_t p_ram_address) const {
	return p_ram_address - m_rom_ram_diff;
}

std::size_t skc::SKC_Config::get_ram_address_from_rom(std::size_t p_rom_address) const {
	return p_rom_address + m_rom_ram_diff;
}

std::pair<byte, byte> skc::SKC_Config::get_ram_address_bytes_from_rom(std::size_t p_rom_address) const {
	std::size_t l_ram_address{ get_ram_address_from_rom(p_rom_address) };
	return std::make_pair(static_cast<byte>(l_ram_address / 256), static_cast<byte>(l_ram_address % 256));
}

std::size_t skc::SKC_Config::get_level_tileset(std::size_t p_level_no, byte p_tileset_no) const {
	return m_level_palettes.at(p_level_no) * 3 + p_tileset_no;
}

const std::string& skc::SKC_Config::get_description(std::size_t p_element_type,
	byte p_element_no) const {
	return m_descriptions.at(p_element_type).at(p_element_no);
}

const std::vector<std::pair<std::string, std::vector<byte>>>& skc::SKC_Config::get_tile_picker(std::size_t p_element_type) const {
	return m_tile_pickers.at(p_element_type);
}

const std::map<std::size_t, std::pair<byte, std::size_t>>& skc::SKC_Config::get_item_bitmasks(void) const {
	return m_item_bitmasks;
}

// metadata items
skc::Metadata_item::Metadata_item(std::size_t p_level_no, std::size_t p_tile_no, const std::string& p_description,
	bool p_transparent, std::size_t p_rom_offset, const std::pair<int, int>& p_position) :
	m_level_no{ p_level_no }, m_tile_no{ p_tile_no }, m_description{ p_description }, m_transparent{
	p_transparent
},
m_rom_offset{ p_rom_offset }, m_position{ p_position }
{ }

std::size_t skc::SKC_Config::get_meta_tile_count(void) const {
	return m_meta_items.size();
}

std::size_t skc::SKC_Config::get_meta_tile_level_no(std::size_t p_index) const {
	return m_meta_items.at(p_index).m_level_no;
}

std::size_t skc::SKC_Config::get_meta_tile_rom_offset(std::size_t p_index) const {
	return m_meta_items.at(p_index).m_rom_offset;
}

std::size_t skc::SKC_Config::get_meta_tile_tile_no(std::size_t p_index) const {
	return m_meta_items.at(p_index).m_tile_no;
}

bool skc::SKC_Config::get_meta_tile_transparent(std::size_t p_index) const {
	return m_meta_items.at(p_index).m_transparent;
}

std::pair<int, int> skc::SKC_Config::get_meta_tile_position(std::size_t p_index) const {
	return m_meta_items.at(p_index).m_position;
}

bool skc::SKC_Config::get_meta_tile_movable(std::size_t p_index) const {
	return get_meta_tile_rom_offset(p_index) != 0;
}

const std::string& skc::SKC_Config::get_meta_tile_description(std::size_t p_index) const {
	return m_meta_items.at(p_index).m_description;
}

void skc::SKC_Config::add_message(const std::string& p_message, int p_msg_type) {
	m_messages.push_front(std::make_pair(p_message, p_msg_type));
	if (m_messages.size() > c::LOG_MESSAGE_MAX_SIZE)
		m_messages.pop_back();
}

const std::deque<std::pair<std::string, int>>& skc::SKC_Config::get_messages(void) const {
	return m_messages;
}

std::string skc::SKC_Config::get_base_path(void) const {
	return m_base_dir;
}

std::string skc::SKC_Config::get_file_path(void) const {
	return m_file_dir;
}

std::string skc::SKC_Config::get_imgui_ini_file_path(void) const {
	return path_combine(get_base_path(), c::FILENAME_IMGUI_INI);
}

std::string skc::SKC_Config::get_config_xml_full_path(void) const {
	std::string l_exe_config_path{ path_combine(m_base_dir, c::FILENAME_CONFIG_XML) };

	if (!std::filesystem::exists(l_exe_config_path))
		return path_combine("./", c::FILENAME_CONFIG_XML);
	else
		return l_exe_config_path;
}

std::string skc::SKC_Config::path_combine(const std::string& p_folder, const std::string& p_filename) {
	return p_folder + p_filename;
}
