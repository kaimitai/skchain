#ifndef SKC_CONFIG_H
#define SKC_CONFIG_H

#include <map>
#include <string>
#include <utility>
#include <vector>
using byte = unsigned char;

namespace skc {

	struct Metadata_item {
		std::string m_description;
		std::size_t m_level_no, m_rom_offset, m_tile_no;
		bool m_transparent;
		std::pair<int, int> m_position;
		Metadata_item(std::size_t p_level_no, std::size_t p_tile_no, const std::string& p_description,
			bool p_transparent, std::size_t p_rom_offset, const std::pair<int, int>& p_position);
	};

	class SKC_Config {
		std::size_t m_offset_gfx, m_offset_blocks, m_offset_enemy_table, m_offset_item_table,
			m_offset_mirror_rate_table, m_offset_mirror_enemy_table;
		unsigned int m_level_count, m_mirror_rate_count, m_mirror_enemy_count, m_gfx_tile_count;
		int m_rom_ram_diff;
		void load_config_xml(void);
		std::vector<byte> m_rom_data;
		std::vector<std::size_t> m_level_palettes;
		std::vector<std::vector<std::string>> m_descriptions;
		std::vector<std::vector<std::pair<std::string, std::vector<byte>>>> m_tile_pickers;
		std::map<std::size_t, std::pair<byte, std::size_t>> m_item_bitmasks;
		std::vector<Metadata_item> m_meta_items;

	public:
		SKC_Config(const std::vector<byte>& p_rom_data);
		const std::vector<byte>& get_rom_data(void) const;

		std::size_t get_offset_gfx(void) const;
		std::size_t get_offset_block_data(std::size_t p_level_no = 0) const;
		std::size_t get_offset_enemy_table_lo(void) const;
		std::size_t get_offset_enemy_table_hi(void) const;
		std::size_t get_offset_item_table_lo(void) const;
		std::size_t get_offset_item_table_hi(void) const;
		std::size_t get_offset_enemy_data(void) const;
		std::size_t get_offset_item_data(void) const;
		std::size_t get_offset_enemy_data(std::size_t p_level_no) const;
		std::size_t get_offset_item_data(std::size_t p_level_no) const;
		std::size_t get_offset_mirror_rate_data(std::size_t p_index) const;
		std::size_t get_offset_mirror_enemy_data(std::size_t p_index) const;
		std::size_t get_offset_mirror_enemy_table_lo(void) const;
		std::size_t get_offset_mirror_enemy_table_hi(void) const;
		unsigned int get_level_count(void) const;
		unsigned int get_nes_tile_count(void) const;
		unsigned int get_mirror_rate_count(void) const;
		unsigned int get_mirror_enemy_count(void) const;
		std::size_t get_rom_address_from_ram(std::size_t p_ram_address) const;
		std::size_t get_ram_address_from_rom(std::size_t p_rom_address) const;
		std::pair<byte, byte> get_ram_address_bytes_from_rom(const std::size_t p_rom_address) const;
		std::size_t get_level_tileset(std::size_t p_level_no, byte p_tileset_no) const;

		const std::vector<std::pair<std::string, std::vector<byte>>>& get_tile_picker(std::size_t p_element_type) const;
		const std::string& get_description(std::size_t p_element_type,
			byte p_element_no) const;
		const std::map<std::size_t, std::pair<byte, std::size_t>>& get_item_bitmasks(void) const;

		// metadata items
		std::size_t get_meta_tile_count(void) const;
		std::size_t get_meta_tile_level_no(std::size_t p_index) const;
		std::size_t get_meta_tile_rom_offset(std::size_t p_index) const;
		std::size_t get_meta_tile_tile_no(std::size_t p_index) const;
		bool get_meta_tile_transparent(std::size_t p_index) const;
		std::pair<int, int> get_meta_tile_position(std::size_t p_index) const;
		bool get_meta_tile_movable(std::size_t p_index) const;
		const std::string& get_meta_tile_description(std::size_t p_index) const;

	};

}

#endif
