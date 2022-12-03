#ifndef SKC_CONFIG_H
#define SKC_CONFIG_H

#include <map>
#include <string>
#include <vector>
using byte = unsigned char;

namespace skc {

	class SKC_Config {
		std::size_t m_offset_gfx, m_offset_blocks, m_offset_enemy_table, m_offset_item_table;
		unsigned int m_level_count, m_gfx_tile_count;
		int m_rom_ram_diff;
		void load_config_xml(void);
		std::vector<byte> m_rom_data;
		std::vector<std::size_t> m_level_tilesets;
		std::vector<std::vector<std::string>> m_descriptions;
		std::vector<std::vector<std::pair<std::string, std::vector<byte>>>> m_tile_pickers;

	public:
		SKC_Config(const std::vector<byte>& p_rom_data);
		const std::vector<byte>& get_rom_data(void) const;

		std::size_t get_offset_gfx(void) const;
		std::size_t get_offset_block_data(void) const;
		std::size_t get_offset_enemy_table_lo(void) const;
		std::size_t get_offset_enemy_table_hi(void) const;
		std::size_t get_offset_item_table_lo(void) const;
		std::size_t get_offset_item_table_hi(void) const;
		std::size_t get_offset_enemy_data(void) const;
		std::size_t get_offset_item_data(void) const;
		unsigned int get_level_count(void) const;
		unsigned int get_nes_tile_count(void) const;
		std::size_t get_rom_address_from_ram(std::size_t p_ram_address) const;
		std::size_t get_ram_address_from_rom(std::size_t p_rom_address) const;
		std::size_t get_level_tileset(std::size_t p_level_no) const;

		const std::vector<std::pair<std::string, std::vector<byte>>>& get_tile_picker(std::size_t p_element_type) const;
		const std::string& get_description(std::size_t p_element_type,
			byte p_element_no) const;
	};

}

#endif
